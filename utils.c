#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void concat(char* to, int n, ...)
{
   va_list vl;
   va_start(vl, n);
   for(int i=0; i<n; ++i) strcat(to, va_arg(vl, const char*));
   va_end(vl);
}
