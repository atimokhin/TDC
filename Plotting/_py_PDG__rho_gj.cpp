/**
 * This is an example file for wrapping C++ functions generating 
 * PlotData1D object
 */
// include file with  PlotData1D python wrapper function definitions
#include "ATbase_python.h" 

#include "PDG__rho_gj.h"

#include <iostream>

/**
 * Wrapper function: It wraps a C++ function which returns a
 * PlotData1D objects.  It gets the same arguments as its C++
 * counterpart and returns a Python objects representing PlotData1D
 */
static PyObject* PDG__rho_gj(PyObject *, PyObject* args)
{
  // First, extract the arguments from a Python tuple
  char* inputfile_name;
  int   n_points;
  int ok = PyArg_ParseTuple( args, "si", &inputfile_name, &n_points );
  if(!ok) return NULL;

  // Call the C++ function and create a new PlotData1D object
  PlotData1D* p_pd = new PlotData1D(PDG__rho_gj(inputfile_name, n_points));
  
  // wrap the pointer as a "PyCObject" and
  // return that object to the interpreter
  return PyCObject_FromVoidPtr( p_pd, delete_PlotData1D);
};


/**
 * Dictionary with functions exported to the Python Interpreter
 * aka method table
 */
static PyMethodDef _py_PDG__rho_gjMethods[] = 
{
    { "PDG__rho_gj", PDG__rho_gj, 
      METH_VARARGS, 
      "PDG__rho_gj(inputfile_name, n_points) -> return PlotData1D object with n points" },

    {NULL,NULL}
};


/**
 * init method of the resulting Python extension module
 * 
 */
PyMODINIT_FUNC  
init_py_PDG__rho_gj(void)
{
  PyObject *m = 
    Py_InitModule4( "_py_PDG__rho_gj",         // name of the module
                    _py_PDG__rho_gjMethods,    // name of the method table
                    "Python interface to PDG__rho_gj(n) function", // doc string for module
                    0, // last two never change
                    PYTHON_API_VERSION );
  return;
}

