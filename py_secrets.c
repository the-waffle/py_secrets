#include "include/Python.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>

/* Macros for accessing PyObject attributes */

#define ob_type(o) (long long) (((PyObject*)(o))->ob_type)
#define ob_size(o) (unsigned long long) (((PyVarObject*)(o))->ob_size)
#define ob_ref_cnt(o) (unsigned long long) (((PyObject*)(o))->ob_refcnt)

#define str_buff (unsigned int) (_PySys_GetSizeOf(Py_BuildValue("s", "1")) - \
                                    _PySys_GetSizeOf(Py_BuildValue("s", "")))
#define bytes_buff (unsigned int) (_PySys_GetSizeOf(Py_BuildValue("y", "1")) - \
                                    _PySys_GetSizeOf(Py_BuildValue("y", "")))
#define long_buff (unsigned int) (_PySys_GetSizeOf(Py_BuildValue("i", 1)) - \
                                    _PySys_GetSizeOf(Py_BuildValue("i", 0)))
#define str_struct_buff (unsigned int) (_PySys_GetSizeOf(Py_BuildValue("s", "")))
#define bytes_struct_buff (unsigned int) (_PySys_GetSizeOf(Py_BuildValue("y", "")))
#define long_struct_buff (unsigned int) (_PySys_GetSizeOf(Py_BuildValue("i", 0)))

/* Function docstrings */

static char zero_str_doc[] = 
    "Zeroes out str content from memory.\n\n:param str: String to be zeroed.\n:returns: Exit code None type if the operation was successful."
;

static char zero_bytes_doc[] = 
    "Zeroes out bytes content from memory.\n\n:param str: Bytes to be zeroed.\n:returns: Exit code None type if the operation was successful."
;

static char zero_int_doc[] = 
    "Zeroes out int content from memory.\n\n:param int: Integer to be zeroed.\n:returns: Exit code None type if the operation was successful."
;

/* Module version */

static PyObject* version(PyObject* self) {
    return Py_BuildValue("s", "Alpha 0.01");
}

/* C Function declaration */

int c_zero_str(PyObject* string) {

/* Zeroes out a str object.
 * 
 * The function uses the ob_size of the object to determine 
 * the size of the object to be zeroed out and finds the empty 
 * struct size of the object to find the offset of the values 
 * in memory. 
 * 
 * The function checks for zero length argument and arguments
 * that will cause an integer overflow when used to determine
 * the size of the memory block to be memset.  
 */

    // Checks for zero length. 
    if (ob_size(string)==str_struct_buff) 
        return 3;

    // Checks for potential overflow. 
    if (ob_size(string) > (ULLONG_MAX / str_buff))
        return 4;
    
    unsigned long long addr = (unsigned long long) string + str_struct_buff - 1;
    unsigned long long clr_size = ob_size(string) * str_buff + 1;
    
    memset(addr, 0, clr_size);
    return 0;
}

int c_zero_bytes(PyObject* bytes) {

/* Zeroes out a bytes object.
 * 
 * The function uses the ob_size of the object to determine 
 * the size of the object to be zeroed out and finds the empty 
 * struct size of the object to find the offset of the values 
 * in memory. 
 * 
 * The function checks for zero length argument and arguments
 * that will cause an integer overflow when used to determine
 * the size of the memory block to be memset. 
 */

    // Checks for zero length. 
    if (ob_size(bytes)==bytes_struct_buff)
        return 3;

    // Checks for potential overflow. 
    if (ob_size(bytes) > (ULLONG_MAX / bytes_buff))
        return 4;

    unsigned long long addr = (unsigned long long) bytes + bytes_struct_buff - 1;
    unsigned long long clr_size = ob_size(bytes) * bytes_buff + 1;

    memset(addr, 0, clr_size);
    return 0;
}

int c_zero_int(PyObject* integer) {

/* Zeroes out a bytes object.
 * 
 * The function uses the ob_size of the object to determine 
 * the size of the object to be zeroed out and finds the empty 
 * struct size of the object to find the offset of the values 
 * in memory. 
 * 
 * The function checks for cached values, negative/signed integers 
 * and arguments that will cause an integer overflow when used to 
 * determine the size of the memory block to be memset. 
 */

    // Checks for signed values. 
    if (PyObject_RichCompare(integer, Py_BuildValue("i", 0), Py_LT)==Py_True)
        return 4;

    // Checks for cached values. 
    if (PyObject_RichCompare(integer, Py_BuildValue("i", 256), Py_LE)==Py_True)
        return 3;

    // Checks for potential overflow. 
    if (ob_size(integer) > (ULLONG_MAX / long_buff))
        return 5;

    unsigned long long addr = (unsigned long long) integer + long_struct_buff;
    unsigned long long clr_size = ob_size(integer) * long_buff;

    memset(addr, 0, clr_size);
    return 0;
}

/* Wrapper function declaration */


static PyObject* zero_bytes(PyObject* self, PyObject* arg) {

/* Zeroes out a bytes object.
 * 
 * This is a wrapper function that calls a C function to 
 * zero out the bytes object in memory. It is recommended that 
 * the object is then put through the del statement after being 
 * zeroed out to be collected by Python GC.
 * 
 * Object type checks and null arguments are done here. 
 */

    Py_IncRef(arg);
    int error = 0;
    char *message = NULL;
    PyObject *type;

    if (PyBytes_Check(arg)!=1)
        // Checks for arg object type.
        error = 1;
    else
        if (arg==NULL)
            // Checks if arg is null. 
            error = 2;
    else
        error = c_zero_bytes(arg);

    switch (error) {
        case 0:
            Py_DecRef(arg);
            Py_RETURN_NONE;
            break;
        case 1:
            message = (char *) calloc(1, sizeof("Argument isn't bytes type. "));
            strncpy(message, "Argument isn't bytes type. ", sizeof("Argument isn't bytes type. "));
            type = PyExc_ValueError;
            break;
        case 2:
            message = (char *) calloc(1, sizeof("Argument was NULL. "));
            strncpy(message, "Argument was NULL. ", sizeof("Argument was NULL. "));
            type = PyExc_ValueError;
            break;
        case 3:
            message = (char *) calloc(1, sizeof("Argument was of zero length. "));
            strncpy(message, "Argument was of zero length. ", sizeof("Argument was of zero length. "));
            type = PyExc_ValueError;
            break;
        case 4:
            message = (char *) calloc(1, sizeof("Argument will cause integer overflow. "));
            strncpy(message, "Argument  will cause integer overflow. ", sizeof("Argument will cause integer overflow. "));
            type = PyExc_OverflowError;
        default:
            message = (char *) calloc(1, sizeof("Unknown exception. "));
            strncpy(message, "Unknown exception. ", sizeof("Unknown exception. "));
            type = PyExc_UserWarning;
    }
    Py_DecRef(arg);
    PyErr_SetString(type, message);
    free(message); message = NULL;
    free(type); type = NULL;
    return NULL;
}


static PyObject* zero_str(PyObject* self, PyObject* arg) {

/* Zeroes out a str object.
 * 
 * This is a wrapper function that calls a C function to 
 * zero out the str object in memory. It is recommended that 
 * the object is then put through the del statement after being 
 * zeroed out to be collected by Python GC.
 * 
 * Object type checks and null arguments are done here. 
 */

    Py_IncRef(arg);
    int error = 0;
    char *message = NULL;
    PyObject *type;

    if (PyUnicode_Check(arg)!=1)
        // Checks for arg object type. 
        error = 1;
    else
        if (arg==NULL)
            // Checks if arg is null. 
            error = 2;
    else
        error = c_zero_str(arg);

    switch (error) {
        case 0:
            Py_DecRef(arg);
            Py_RETURN_NONE;
            break;
        case 1:
            message = (char *) calloc(1, sizeof("Argument isn't str type. "));
            strncpy(message, "Argument isn't str type. ", sizeof("Argument isn't str type. "));
            type = PyExc_ValueError;
            break;
        case 2:
            message = (char *) calloc(1, sizeof("Argument was NULL. "));
            strncpy(message, "Argument was NULL. ", sizeof("Argument was NULL. "));
            type = PyExc_ValueError;
            break;
        case 3:
            message = (char *) calloc(1, sizeof("Argument was of zero length. "));
            strncpy(message, "Argument was of zero length. ", sizeof("Argument was of zero length. "));
            type = PyExc_ValueError;
            break;
        case 4:
            message = (char *) calloc(1, sizeof("Argument will cause integer overflow. "));
            strncpy(message, "Argument will cause integer overflow. ", sizeof("Argument will cause integer overflow. "));
            type = PyExc_OverflowError;
        default:
            message = (char *) calloc(1, sizeof("Unknown exception. "));
            strncpy(message, "Unknown exception. ", sizeof("Unknown exception. "));
            type = PyExc_UserWarning;
    }
    Py_DecRef(arg);
    PyErr_SetString(type, message);
    free(message); message = NULL;
    free(type); type = NULL;
    return NULL;
}

static PyObject* zero_int(PyObject* self, PyObject* arg) {

/* Zeroes out a int object.
 * 
 * This is a wrapper function that calls a C function to 
 * zero out the int object in memory. It is recommended that 
 * the object is then put through the del statement after being 
 * zeroed out to be collected by Python GC.
 * 
 * Object type checks and null arguments are done here. 
 */

    Py_IncRef(arg);
    int error = 0;
    char *message = NULL;
    PyObject *type;

    if (PyLong_Check(arg)!=1)
        // Checks for arg type. 
        error = 1;
    else 
        if (arg==NULL)
            // Checks if arg is null. 
            error = 2;
    else
        error = c_zero_int(arg);

    switch (error) {
        case 0:
            Py_DecRef(arg);
            Py_RETURN_NONE;
            break;
        case 1:
            message = (char *) calloc(1, sizeof("Argument isn't int type. "));
            strncpy(message, "Argument isn't int type. ", sizeof("Argument isn't int type. "));
            type = PyExc_ValueError;
            break;
        case 2:
            message = (char *) calloc(1, sizeof("Argument was NULL. "));
            strncpy(message, "Argument was NULL. ", sizeof("Argument was NULL. "));
            type = PyExc_ValueError;
            break;
        case 3:
            message = (char *) calloc(1, sizeof("Argument was a preallocated integer between 0 & 256. "));
            strncpy(message, "Argument was a preallocated integer between 0 & 256. ", sizeof("Argument was a preallocated integer between 0 & 256. "));
            type = PyExc_ValueError;
            break;
        case 4:
            message = (char *) calloc(1, sizeof("Argument was a signed integer. "));
            strncpy(message, "Argument was a signed integer. ", sizeof("Argument was a signed integer. "));
            type = PyExc_ValueError;
            break;
        case 5:
            message = (char *) calloc(1, sizeof("Argument will cause integer overflow. "));
            strncpy(message, "Argument will cause integer overflow. ", sizeof("Argument will cause integer overflow. "));
            type = PyExc_OverflowError;
        default:
            message = (char *) calloc(1, sizeof("Unknown exception. "));
            strncpy(message, "Unknown exception. ", sizeof("Unknown exception. "));
            type = PyExc_UserWarning;
    }
    Py_DecRef(arg);
    PyErr_SetString(type, message);
    free(message); message = NULL;
    free(type); type = NULL;
    return NULL;
}

static PyMethodDef module_funcs[] = {

    {"zero_str", (PyCFunction)zero_str,
    METH_O, zero_str_doc},

    {"zero_bytes", (PyCFunction)zero_bytes,
    METH_O, zero_bytes_doc},

    {"zero_int", (PyCFunction)zero_int,
    METH_O, zero_int_doc},

    {"version", (PyCFunction)version,
    METH_NOARGS, "returns the version. "},

    {NULL, NULL, 0, NULL}
};


/* Module build values */

static struct PyModuleDef py_secrets = {
    PyModuleDef_HEAD_INIT,
    "py_secrets",
    "Python C extension module for safe management and removal of secrets and sensitive data in memory. ",
    -1,
    module_funcs
};

PyMODINIT_FUNC PyInit_py_secrets(void) {
    return PyModule_Create(&py_secrets);
}
