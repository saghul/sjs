/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.
   Copyright (C) 2005, Mutsuo Saito
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
    Changes from the original by Saúl Ibarra Corretgé <saghul@gmail.com>
        - Moved state to a typedef
        - Switched types to uint32_t
        - Formatting
*/

#include <stdint.h>


/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

/* State */
typedef struct {
    uint32_t state[N];
    int index;
} mt_t;

/* initializes mt[N] with a seed */
void mt_init(mt_t* mtt, uint32_t s);

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void mt_init_by_array(mt_t* mtt, uint32_t init_key[], unsigned int key_length);

/* generates a random number on [0,0xffffffff]-interval */
uint32_t mt_genrand_int32(mt_t* mtt);

/* generates a random number on [0,0x7fffffff]-interval */
uint32_t mt_genrand_int31(mt_t* mtt);

/* These real versions are due to Isaku Wada, 2002/01/09 added */
/* generates a random number on [0,1]-real-interval */
double mt_genrand_real1(mt_t* mtt);

/* generates a random number on [0,1)-real-interval */
double mt_genrand_real2(mt_t* mtt);

/* generates a random number on (0,1)-real-interval */
double mt_genrand_real3(mt_t* mtt);

/* generates a random number on [0,1) with 53-bit resolution*/
double mt_genrand_res53(mt_t* mtt);
