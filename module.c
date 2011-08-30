#include "module.h"
#include <Python.h>
#include <stdio.h>

void python_module_init()
{
	Py_Initialize();
}

void python_module_call(struct python_module* p, const char* script, const char* func)
{
	python_module_init();
	
	FILE* f=fopen(script, "r");
	PyRun_SimpleFile(f, script);
	fclose(f);
	// Get script from its name
	p->pModule=PyImport_AddModule("__main__");

	if(p->pModule)
	{
		p->pDict=PyModule_GetDict(p->pModule);
		// Get function name
		p->pFunc=PyDict_GetItemString(p->pDict, func);
		Py_XDECREF(p->pDict);
		if(p->pFunc && PyCallable_Check(p->pFunc))
		{
			p->pArgs=PyTuple_New(0);
			p->pValue=PyObject_CallObject(p->pFunc, p->pArgs);
			if(p->pValue)
			{
				printf("Returned: %s\n", PyString_AsString(p->pValue));
				Py_DECREF(p->pValue);
			}
			else
			{
				Py_DECREF(p->pFunc);
				Py_DECREF(p->pModule);
				PyErr_Print();
				fprintf(stderr, "Python module failed!!\n");
			}
		}
		else
		{
			if(PyErr_Occurred()) PyErr_Print();
			fprintf(stderr, "Cannot find python function: %s\n", func);
		}
		Py_XDECREF(p->pFunc);
	}
	else
	{
		PyErr_Print();
		fprintf(stderr, "Failed to load %s\n", script);
	}
	Py_Finalize();
}
