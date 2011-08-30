#ifndef MODULE_H
#define MODULE_H

#ifdef CBOT_PYTHON
struct python_module
{
	PyObject *pName, *pModule, *pDict, *pFunc, *pArgs, *pValue;
};

void python_module_init();
void python_module_start(struct python_module* p, const char* script)
#endif

#endif
