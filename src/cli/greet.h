
#ifndef SJS_GREET_H
#define SJS_GREET_H

#include "../embedjs.h"

INCBIN(greet, "embed-js/greet.js");

/**
 * These are defined now:
 *
 * const unsigned char sjs__code_greet_data[];
 * const unsigned char *const sjs__code_greet_end;
 * const unsigned int sjs__code_greet_size;
 *
 */

#endif
