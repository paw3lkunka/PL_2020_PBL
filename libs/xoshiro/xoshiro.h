#ifndef XOR_SHIFT_ROTATE
#define XOR_SHIFT_ROTATE

#include <stdint.h>

   #ifdef __cplusplus
   extern "C" {
   #endif

   /**
    * @brief Initialize xoshiro RNG.
    */
   void xoshiro_Init();

   /**
    * @brief Generate random number;
    * @return uint64_t 
    */
   uint64_t xoshiro_next(void);

   /* This is the jump function for the generator. It is equivalent
      to 2^128 calls to next(); it can be used to generate 2^128
      non-overlapping subsequences for parallel computations. */
   void xoshiro_jump(void);

   /* This is the long-jump function for the generator. It is equivalent to
      2^192 calls to next(); it can be used to generate 2^64 starting points,
      from each of which jump() will generate 2^64 non-overlapping
      subsequences for parallel distributed computations. */
   void xoshiro_long_jump(void);
   
   #ifdef __cplusplus
   }
   #endif

#endif