#pragma once 



#ifndef RT_FUNCTION
   #define RT_FUNCTION __forceinline__ __host__ __device__ 
#endif

#ifndef RT_FUNCTION_DEVICE
   #define RT_FUNCTION_DEVICE __forceinline__ __device__ 
#endif

#ifndef RT_FUNCTION_HOST
   #define RT_FUNCTION_HOST __forceinline__ __host__ 
#endif
