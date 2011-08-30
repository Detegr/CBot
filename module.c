#include "module.h"

void python_module_init()
{
	Py_Initialize();
}

void python_module_start(struct python_module* p, const char* script)
{
	python_module_init();
	
	// Get script from its name.
	pName=PyString_FromString(script);
	pModule=PyImport_Import(pName);
	Py_DECREF(pName);

}
