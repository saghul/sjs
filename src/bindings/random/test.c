
#include <stdio.h>
#include "mt19937ar.h"


void main(void){
    mt_t mt;
    uint32_t init[4] = {61731, 24903, 614, 42143};
    unsigned int length = 4;

    mt_init_by_array(&mt, init, length);
    for (int i = 0; i < 2000; i++) {
        printf("%.16f ", mt_genrand_res53(&mt));
        if (i%5==4)
            printf("\n");
    }
}
