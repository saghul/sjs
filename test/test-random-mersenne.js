'use strict';

const assert = require('assert');
const random = require('random');


// Compare our implementation with results from the original
// code.  Create 2000 53-bit precision random floats.  Compare only
// the last ten entries to show that the independent implementations
// are tracking.  Here is the main() function needed to create the
// list of expected random numbers:
//    void main(void){
//         int i;
//         unsigned long init[4]={61731, 24903, 614, 42143}, length=4;
//         init_by_array(init, length);
//         for (i=0; i<2000; i++) {
//           printf("%.15f ", genrand_res53());
//           if (i%5==4) printf("\n");
//         }
//     }

const expected = [0.4583980307371326,
                  0.8605781520197878,
                  0.9284833172678215,
                  0.3593268111978246,
                  0.08182349376244957,
                  0.1433222647016933,
                  0.08429782382352002,
                  0.5381486467183145,
                  0.0892150249119934,
                  0.7848619610537291];


random.seed([61731, 24903, 614, 42143]);

var r = [];
for (var i = 0; i < 2000; i++) {
	r.push(random.random());
}
assert.deepEqual(r.slice(-10), expected);
