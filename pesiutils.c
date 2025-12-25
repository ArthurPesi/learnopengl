#include <stdio.h>
#define p_assert(expression) if(!(expression)) { printf("Runtime error:\n%d | assert(" #expression ") returned false\n", __LINE__); __builtin_trap(); }

#define throwError(errorMessage) { printf("Runtime error:\n%d | %s\n", __LINE__, errorMessage); __builtin_trap(); }
