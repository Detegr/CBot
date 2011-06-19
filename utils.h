#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
static void concat(char* to, int n, ...)
{
   va_list vl;
   va_start(vl, n);
   for(int i=0; i<n; ++i) strcat(to, va_arg(vl, const char*));
   va_end(vl);
}

#endif
