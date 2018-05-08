
#ifndef SJS_POLYFILL_H
#define SJS_POLYFILL_H

#include "embedjs.h"

INCBIN(polyfill, "embed-js/polyfill.min.js");

/**
 * These are defined now:
 *
 * const unsigned char sjs__code_polyfill_data[];
 * const unsigned char *const sjs__code_polyfill_end;
 * const unsigned int sjs__code_polyfill_size;
 *
 */

#endif
