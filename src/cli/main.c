/*
 *  Command line execution tool.  Useful for test cases and manual testing.
 *
 *  To enable readline and other fancy stuff, compile with -DDUK_CMDLINE_FANCY.
 *  It is not the default to maximize portability.  You can also compile in
 *  support for example allocators, grep for DUK_CMDLINE_*.
 */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || \
    defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
/* Suppress warnings about plain fopen() etc. */
#define _CRT_SECURE_NO_WARNINGS
#if defined(_MSC_VER) && (_MSC_VER < 1900)
/* Workaround for snprintf() missing in older MSVC versions.
 * Note that _snprintf() may not NUL terminate the string, but
 * this difference does not matter here as a NUL terminator is
 * always explicitly added.
 */
#define snprintf _snprintf
#endif
#endif

#define  GREET_CODE(variant)  \
	"print('((o) Duktape" variant " ' + " \
	"Math.floor(Duktape.version / 10000) + '.' + " \
	"Math.floor(Duktape.version / 100) % 100 + '.' + " \
	"Duktape.version % 100" \
	", '(" DUK_GIT_DESCRIBE ")');"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "duktape.h"
#include "linenoise.h"


static int interactive_mode = 0;

static int get_stack_raw(duk_context *ctx) {
	if (!duk_is_object(ctx, -1)) {
		return 1;
	}
	if (!duk_has_prop_string(ctx, -1, "stack")) {
		return 1;
	}
	if (!duk_is_error(ctx, -1)) {
		/* Not an Error instance, don't read "stack". */
		return 1;
	}

	duk_get_prop_string(ctx, -1, "stack");  /* caller coerces */
	duk_remove(ctx, -2);
	return 1;
}

/* Print error to stderr and pop error. */
static void print_pop_error(duk_context *ctx, FILE *f) {
	/* Print error objects with a stack trace specially.
	 * Note that getting the stack trace may throw an error
	 * so this also needs to be safe call wrapped.
	 */
	(void) duk_safe_call(ctx, get_stack_raw, 1 /*nargs*/, 1 /*nrets*/);
	fprintf(f, "%s\n", duk_safe_to_string(ctx, -1));
	fflush(f);
	duk_pop(ctx);
}

static int wrapped_compile_execute(duk_context *ctx) {
	const char *src_data;
	duk_size_t src_len;
	int comp_flags;

	/* XXX: Here it'd be nice to get some stats for the compilation result
	 * when a suitable command line is given (e.g. code size, constant
	 * count, function count.  These are available internally but not through
	 * the public API.
	 */

	/* Use duk_compile_lstring_filename() variant which avoids interning
	 * the source code.  This only really matters for low memory environments.
	 */

	/* [ ... bytecode_filename src_data src_len filename ] */

	src_data = (const char *) duk_require_pointer(ctx, -3);
	src_len = (duk_size_t) duk_require_uint(ctx, -2);

	if (src_data != NULL && src_len >= 2 && src_data[0] == (char) 0xff) {
		/* Bytecode. */
		duk_push_lstring(ctx, src_data, src_len);
		duk_to_buffer(ctx, -1, NULL);
		duk_load_function(ctx);
	} else {
		/* Source code. */
		comp_flags = 0;
		duk_compile_lstring_filename(ctx, comp_flags, src_data, src_len);
	}

	/* [ ... bytecode_filename src_data src_len function ] */

	/* Optional bytecode dump. */
	if (duk_is_string(ctx, -4)) {
		FILE *f;
		void *bc_ptr;
		duk_size_t bc_len;
		size_t wrote;
		char fnbuf[256];
		const char *filename;

		duk_dup_top(ctx);
		duk_dump_function(ctx);
		bc_ptr = duk_require_buffer(ctx, -1, &bc_len);
		filename = duk_require_string(ctx, -5);
		snprintf(fnbuf, sizeof(fnbuf), "%s", filename);
		fnbuf[sizeof(fnbuf) - 1] = (char) 0;

		f = fopen(fnbuf, "wb");
		if (!f) {
			duk_error(ctx, DUK_ERR_ERROR, "failed to open bytecode output file");
		}
		wrote = fwrite(bc_ptr, 1, (size_t) bc_len, f);  /* XXX: handle partial writes */
		(void) fclose(f);
		if (wrote != bc_len) {
			duk_error(ctx, DUK_ERR_ERROR, "failed to write all bytecode");
		}

		return 0;  /* duk_safe_call() cleans up */
	}

#if 0
	/* Manual test for bytecode dump/load cycle: dump and load before
	 * execution.  Enable manually, then run "make qecmatest" for a
	 * reasonably good coverage of different functions and programs.
	 */
	duk_dump_function(ctx);
	duk_load_function(ctx);
#endif

	duk_push_global_object(ctx);  /* 'this' binding */
	duk_call_method(ctx, 0);

	if (interactive_mode) {
		/*
		 *  In interactive mode, write to stdout so output won't
		 *  interleave as easily.
		 *
		 *  NOTE: the ToString() coercion may fail in some cases;
		 *  for instance, if you evaluate:
		 *
		 *    ( {valueOf: function() {return {}},
		 *       toString: function() {return {}}});
		 *
		 *  The error is:
		 *
		 *    TypeError: failed to coerce with [[DefaultValue]]
		 *            duk_api.c:1420
		 *
		 *  These are handled now by the caller which also has stack
		 *  trace printing support.  User code can print out errors
		 *  safely using duk_safe_to_string().
		 */

		fprintf(stdout, "= %s\n", duk_to_string(ctx, -1));
		fflush(stdout);
	} else {
		/* In non-interactive mode, success results are not written at all.
		 * It is important that the result value is not string coerced,
		 * as the string coercion may cause an error in some cases.
		 */
	}

	return 0;  /* duk_safe_call() cleans up */
}

static int handle_fh(duk_context *ctx, FILE *f, const char *filename, const char *bytecode_filename) {
	char *buf = NULL;
	size_t bufsz;
	size_t bufoff;
	size_t got;
	int rc;
	int retval = -1;

	buf = (char *) malloc(1024);
	if (!buf) {
		goto error;
	}
	bufsz = 1024;
	bufoff = 0;

	/* Read until EOF, avoid fseek/stat because it won't work with stdin. */
	for (;;) {
		size_t avail;

		avail = bufsz - bufoff;
		if (avail < 1024) {
			size_t newsz;
#if 0
			fprintf(stderr, "resizing read buffer: %ld -> %ld\n", (long) bufsz, (long) (bufsz * 2));
#endif
			newsz = bufsz + (bufsz >> 2) + 1024;  /* +25% and some extra */
			buf = (char *) realloc(buf, newsz);
			if (!buf) {
				goto error;
			}
			bufsz = newsz;
		}

		avail = bufsz - bufoff;
#if 0
		fprintf(stderr, "reading input: buf=%p bufsz=%ld bufoff=%ld avail=%ld\n",
		        (void *) buf, (long) bufsz, (long) bufoff, (long) avail);
#endif

		got = fread((void *) (buf + bufoff), (size_t) 1, avail, f);
#if 0
		fprintf(stderr, "got=%ld\n", (long) got);
#endif
		if (got == 0) {
			break;
		}
		bufoff += got;
	}

	duk_push_string(ctx, bytecode_filename);
	duk_push_pointer(ctx, (void *) buf);
	duk_push_uint(ctx, (duk_uint_t) bufoff);
	duk_push_string(ctx, filename);

	interactive_mode = 0;  /* global */

	rc = duk_safe_call(ctx, wrapped_compile_execute, 4 /*nargs*/, 1 /*nret*/);

	free(buf);
	buf = NULL;

	if (rc != DUK_EXEC_SUCCESS) {
		print_pop_error(ctx, stderr);
		goto error;
	} else {
		duk_pop(ctx);
		retval = 0;
	}
	/* fall thru */

 cleanup:
	if (buf) {
		free(buf);
	}
	return retval;

 error:
	fprintf(stderr, "error in executing file %s\n", filename);
	fflush(stderr);
	goto cleanup;
}

static int handle_file(duk_context *ctx, const char *filename, const char *bytecode_filename) {
	FILE *f = NULL;
	int retval;
	char fnbuf[256];

	snprintf(fnbuf, sizeof(fnbuf), "%s", filename);
	fnbuf[sizeof(fnbuf) - 1] = (char) 0;

	f = fopen(fnbuf, "rb");
	if (!f) {
		fprintf(stderr, "failed to open source file: %s\n", filename);
		fflush(stderr);
		goto error;
	}

	retval = handle_fh(ctx, f, filename, bytecode_filename);

	fclose(f);
	return retval;

 error:
	return -1;
}

static int handle_eval(duk_context *ctx, char *code) {
	int rc;
	int retval = -1;

	duk_push_pointer(ctx, (void *) code);
	duk_push_uint(ctx, (duk_uint_t) strlen(code));
	duk_push_string(ctx, "eval");

	interactive_mode = 0;  /* global */

	rc = duk_safe_call(ctx, wrapped_compile_execute, 3 /*nargs*/, 1 /*nret*/);

	if (rc != DUK_EXEC_SUCCESS) {
		print_pop_error(ctx, stderr);
	} else {
		duk_pop(ctx);
		retval = 0;
	}

	return retval;
}


static int handle_interactive(duk_context *ctx) {
	const char *prompt = "sjs> ";
	char* line;
	int retval = 0;
	int rc;

	duk_eval_string(ctx, GREET_CODE(""));
	duk_pop(ctx);

	while((line = linenoise(prompt)) != NULL) {
		if (line[0] != '\0') {
		    linenoiseHistoryAdd(line);
		}

		duk_push_pointer(ctx, (void *) line);
		duk_push_uint(ctx, (duk_uint_t) strlen(line));
		duk_push_string(ctx, "input");

		interactive_mode = 1;  /* global */

		rc = duk_safe_call(ctx, wrapped_compile_execute, 3 /*nargs*/, 1 /*nret*/);

		free(line);

		if (rc != DUK_EXEC_SUCCESS) {
			/* in interactive mode, write to stdout */
			print_pop_error(ctx, stdout);
			retval = -1;  /* an error 'taints' the execution */
		} else {
			duk_pop(ctx);
		}
	}

	return retval;
}


static duk_context *create_duktape_heap(void) {
	duk_context *ctx;

	ctx = duk_create_heap_default();
	if (!ctx) {
		fprintf(stderr, "Failed to create Duktape heap\n");
		fflush(stderr);
		exit(-1);
	}

	return ctx;
}

static void destroy_duktape_heap(duk_context *ctx) {
	if (ctx) {
		duk_destroy_heap(ctx);
	}
}

int main(int argc, char *argv[]) {
	duk_context *ctx = NULL;
	int retval = 0;
	int have_files = 0;
	int have_eval = 0;
	int interactive = 0;
	int run_stdin = 0;
	const char *compile_filename = NULL;
	int i;

	/*
	 *  Signal handling setup
	 */

	signal(SIGPIPE, SIG_IGN);

	/*
	 *  Parse options
	 */

	for (i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (!arg) {
			goto usage;
		}
		if (strcmp(arg, "-i") == 0) {
			interactive = 1;
		} else if (strcmp(arg, "-c") == 0) {
			if (i == argc - 1) {
				goto usage;
			}
			i++;
			compile_filename = argv[i];
		} else if (strcmp(arg, "-e") == 0) {
			have_eval = 1;
			if (i == argc - 1) {
				goto usage;
			}
			i++;  /* skip code */
		} else if (strcmp(arg, "--run-stdin") == 0) {
			run_stdin = 1;
		} else if (strlen(arg) >= 1 && arg[0] == '-') {
			goto usage;
		} else {
			have_files = 1;
		}
	}
	if (!have_files && !have_eval && !run_stdin) {
		interactive = 1;
	}

	/*
	 *  Create heap
	 */

	ctx = create_duktape_heap();

	/*
	 *  Execute any argument file(s)
	 */

	for (i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (!arg) {
			continue;
		} else if (strlen(arg) == 2 && strcmp(arg, "-e") == 0) {
			/* Here we know the eval arg exists but check anyway */
			if (i == argc - 1) {
				retval = 1;
				goto cleanup;
			}
			if (handle_eval(ctx, argv[i + 1]) != 0) {
				retval = 1;
				goto cleanup;
			}
			i++;  /* skip code */
			continue;
		} else if (strlen(arg) == 2 && strcmp(arg, "-c") == 0) {
			i++;  /* skip filename */
			continue;
		} else if (strlen(arg) >= 1 && arg[0] == '-') {
			continue;
		}

		if (handle_file(ctx, arg, compile_filename) != 0) {
			retval = 1;
			goto cleanup;
		}
	}

	if (run_stdin) {
		/* Running stdin like a full file (reading all lines before
		 * compiling) is useful with emduk:
		 * cat test.js | ./emduk --run-stdin
		 */
		if (handle_fh(ctx, stdin, "stdin", compile_filename) != 0) {
			retval = 1;
			goto cleanup;
		}
	}

	/*
	 *  Enter interactive mode if options indicate it
	 */

	if (interactive) {
		if (handle_interactive(ctx) != 0) {
			retval = 1;
			goto cleanup;
		}
	}

	/*
	 *  Cleanup and exit
	 */

 cleanup:
	if (interactive) {
		fprintf(stderr, "Cleaning up...\n");
		fflush(stderr);
	}

	destroy_duktape_heap(ctx);
	ctx = NULL;

	return retval;

	/*
	 *  Usage
	 */

 usage:
	fprintf(stderr, "Usage: duk [options] [<filenames>]\n"
	                "\n"
	                "   -i                 enter interactive mode after executing argument file(s) / eval code\n"
	                "   -e CODE            evaluate code\n"
			"   -c FILE            compile into bytecode (use with only one file argument)\n"
			"   --run-stdin        treat stdin like a file, i.e. compile full input (not line by line)\n"
	                "\n"
	                "If <filename> is omitted, interactive mode is started automatically.\n");
	fflush(stderr);
	exit(1);
}
