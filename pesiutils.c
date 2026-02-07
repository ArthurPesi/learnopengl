#include <stdio.h>
#define p_assert(expression) if(!(expression)) { printf("Pesi runtime error:\n%d | assert(" #expression ") returned false\n", __LINE__); __builtin_trap(); }

#define throwError(errorMessage) { printf(" Pesi runtime error:\n%d | %s\n", __LINE__, errorMessage); __builtin_trap(); }
#define M_PI		3.14159265358979323846	/* pi */
#define radians(degrees) ((degrees) * M_PI / 180)
#define degrees(radians) ((radians) * 180 / M_PI)
#define arrayCount(array) (sizeof((array)) / sizeof((array)[0]))

#define local static
#define global_variable static
#define local_persist static
#define true 1
#define false 0

typedef unsigned char bool8;
typedef unsigned short bool16;
typedef unsigned int bool32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef unsigned long uintptr;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef float real32;
typedef double real64;
