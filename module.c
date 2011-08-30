#include "module.h"
#include <Python.h>
#include <stdio.h>

const char* python_module_call(struct python_module* p, const char* script, const char* func)
{
	Py_Initialize();

	const char* ret=NULL;
	if(!p->pModule){
		FILE* f=fopen(script, "r");
		PyRun_SimpleFile(f, script);
		fclose(f);
		p->pModule=PyImport_ImportModule("__main__");
	}

	if(p->pModule)
	{
		p->pDict=PyModule_GetDict(p->pModule);

		// Get function name
		p->pFunc=PyDict_GetItemString(p->pDict, func);
		if(p->pFunc && PyCallable_Check(p->pFunc))
		{
			p->pArgs=PyTuple_New(0);
			p->pValue=PyObject_CallObject(p->pFunc, p->pArgs);
			if(p->pValue)
			{
				ret=PyString_AsString(p->pValue);
				Py_DECREF(p->pValue);
				Py_XDECREF(p->pArgs);
			}
			else
			{
				Py_DECREF(p->pFunc);
				Py_DECREF(p->pModule);
				Py_XDECREF(p->pArgs);
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
	return ret;
}
