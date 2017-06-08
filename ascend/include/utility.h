#ifndef A_UTILITY_H
#define A_UTILITY_H

#include "types.h"

/**    MATH    **/
//  Returns N - N%D
#define NMOD(n,d) ((n)-((n)%(d)))

/**   RANDOM   **/
//  Returns +1 or -1 with 50/50 probability.
signed char rsign();
//  Returns the sum of D random numbers between 0 and N-1
template <typename NUM> NUM rand2(NUM, u_16 = 1);


#endif // A_UTILITY_H
