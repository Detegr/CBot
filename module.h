#ifndef MODULE_H
#define MODULE_H

#ifdef CBOT_PYTHON
#include <Python.h>
struct python_module
{
	PyObject *pName, *pModule, *pDict, *pFunc, *pArgs, *pValue;
};

void python_module_init();
void python_module_call(struct python_module* p, const char* script, const char* func);
#endif

#endif
