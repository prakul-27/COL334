#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include<stdio.h>
#include<netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <regex>

using namespace std;

int main()
{
    // Py_Initialize();
    // PyObject* myModuleString = PyUnicode_FromString("parser");
    // PyObject* myModule = PyImport_Import(myModuleString);
    // PyObject* myFunction = PyObject_GetAttrString(myModule, "dumb");

    // if(myFunction != NULL) {
    //     PyObject* pTup = PyEval_CallObject(myFunction, NULL);
    // } else {
    //     cout << "myFunction returned null\n";
    // }

    // Py_Finalize();
    // return 0;

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
   pFunc = PyDict_GetItemString(pDict, (char*)"dumb");

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
   cout << PyLong_AsLong(presult) << endl;
   Py_DECREF(pValue);

   // Clean up
   Py_DECREF(pModule);
   Py_DECREF(pName);

   // Finish the Python Interpreter
   Py_Finalize();


    return 0;

}