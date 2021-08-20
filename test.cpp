#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include<stdio.h>
#include<netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <regex>
#include <bits/stdc++.h>

using namespace std;

vector<string> listTupleToVector_String(PyObject* incoming) {
	vector<string> data;
	if (PyTuple_Check(incoming)) {
		for(Py_ssize_t i = 0; i < PyTuple_Size(incoming); i++) {
			PyObject *value = PyTuple_GetItem(incoming, i);
			data.push_back( PyUnicode_AsUTF8(value) );
		}
	} else {
		if (PyList_Check(incoming)) {
			for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++) {
				PyObject *value = PyList_GetItem(incoming, i);
				data.push_back( PyUnicode_AsUTF8(value) );
			}
		} else {
			throw logic_error("Passed PyObject pointer was not a list or tuple!");
		}
	}
	return data;
}

int main()
{
    // Set PYTHONPATH TO working directory
   setenv("PYTHONPATH",".",1);

   PyObject *pName, *pModule, *pDict, *pFunc, *pValue, *presult;


   // Initialize the Python Interpreter
   Py_Initialize();


   // Build the name object
   pName = PyUnicode_FromString((char*)"parser");

   // Load the module object
   pModule = PyImport_Import(pName);


   // pDict is a borrowed reference 
   pDict = PyModule_GetDict(pModule);


   // pFunc is also a borrowed reference 
   pFunc = PyDict_GetItemString(pDict, (char*)"parse");

   if (PyCallable_Check(pFunc))
   {
       pValue=Py_BuildValue("(z)",(char*)"something");
    //    PyErr_Print();
    //    printf("Let's give this a shot!\n");
       presult=PyObject_CallObject(pFunc,NULL);
       PyErr_Print();
   } else 
   {
       PyErr_Print();
   }
   vector<string> result = listTupleToVector_String(presult);
   Py_DECREF(pValue);

   // Clean up
   Py_DECREF(pModule);
   Py_DECREF(pName);

   // Finish the Python Interpreter
   Py_Finalize();


    for(auto x : result) {
        cout << x << endl;
    }

    return 0;

}