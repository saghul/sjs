
#ifndef SJS_BOOTSTRAP_H
#define SJS_BOOTSTRAP_H

#include "embedjs.h"

INCBIN(bootstrap, "embed-js/bootstrap.js");

/**
 * These are defined now:
 *
 * const unsigned char sjs__code_bootstrap_data[];
 * const unsigned char *const sjs__code_bootstrap_end;
 * const unsigned int sjs__code_bootstrap_size;
 *
 */

#endif

