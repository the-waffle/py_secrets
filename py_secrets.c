#include "include/Python.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

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

/* Function declaration */

int c_zero_str(PyObject* string) {

    if (ob_size(string)==str_struct_buff) 
        return 3;

    long long addr = (long long) string + str_struct_buff - 1;
    long long clr_size = ob_size(string) * str_buff + 1;

    memset(addr, 0, clr_size);
    return 0;
}

int c_zero_bytes(PyObject* bytes) {

    if (ob_size(bytes)==bytes_struct_buff)
        return 3;

    long long addr = (long long) bytes + bytes_struct_buff - 1;
    long long clr_size = ob_size(bytes) * bytes_buff + 1;

    memset(addr, 0, clr_size);
    return 0;
}

int c_zero_int(PyObject* integer) {

    if (PyObject_RichCompare(integer, Py_BuildValue("i", 0), Py_LT)==Py_True)
        return 4;

    if (PyObject_RichCompare(integer, Py_BuildValue("i", 256), Py_LE)==Py_True)
        return 3;

    long long addr = (long long) integer + long_struct_buff;
    long long clr_size = ob_size(integer) * long_buff;

    memset(addr, 0, clr_size);
    return 0;
}

/* Wrapper function declaration */


static PyObject* zero_bytes(PyObject* self, PyObject* arg) {

    Py_IncRef(arg);
    int error = 0;
    char *message = NULL;
    PyObject *type;

    /* Check if args is str. */

    if (PyBytes_Check(arg)!=1)
        error = 1;
    else
        if (arg==NULL)
            error = 2;
    else
        error = c_zero_bytes(arg);

    switch (error) {
        case 0:
            Py_DecRef(arg);
            Py_RETURN_NONE;
            break;
        case 1:
            message = (char *) malloc(sizeof("Argument isn't bytes type. ") + 1);
            strcpy(message, "Argument isn't bytes type. ");
            type = PyExc_ValueError;
            break;
        case 2:
            message = (char *) malloc(sizeof("Argument was NULL. ") + 1);
            strcpy(message, "Argument was NULL. ");
            type = PyExc_ValueError;
            break;
        case 3:
            message = (char *) malloc(sizeof("Argument was of zero length. ") + 1);
            strcpy(message, "Argument was of zero length. ");
            type = PyExc_ValueError;
            break;
        default:
            message = (char *) malloc(sizeof("Unknown exception. ") + 1);
            strcpy(message, "Unknown exception. ");
            type = PyExc_UserWarning;
    }
    Py_DecRef(arg);
    PyErr_SetString(type, message);
    free(message);
    return NULL;

}


static PyObject* zero_str(PyObject* self, PyObject* arg) {

    Py_IncRef(arg);
    int error = 0;
    char *message = NULL;
    PyObject *type;

    /* Check if args is str. */

    if (PyUnicode_Check(arg)!=1)
        error = 1;
    else
        if (arg==NULL)
            error = 2;
    else
        error = c_zero_str(arg);

    switch (error) {
        case 0:
            Py_DecRef(arg);
            Py_RETURN_NONE;
            break;
        case 1:
            message = (char *) malloc(sizeof("Argument isn't str type. ") + 1);
            strcpy(message, "Argument isn't str type. ");
            type = PyExc_ValueError;
            break;
        case 2:
            message = (char *) malloc(sizeof("Argument was NULL. ") + 1);
            strcpy(message, "Argument was NULL. ");
            type = PyExc_ValueError;
            break;
        case 3:
            message = (char *) malloc(sizeof("Argument was of zero length. ") + 1);
            strcpy(message, "Argument was of zero length. ");
            type = PyExc_ValueError;
            break;
        default:
            message = (char *) malloc(sizeof("Unknown exception. ") + 1);
            strcpy(message, "Unknown exception. ");
            type = PyExc_UserWarning;
    }
    Py_DecRef(arg);
    PyErr_SetString(type, message);
    free(message);
    return NULL;
}

static PyObject* zero_int(PyObject* self, PyObject* arg) {

    Py_IncRef(arg);
    int error = 0;
    char *message = NULL;
    PyObject *type;

    /* Check if arg is int type */

    if (PyLong_Check(arg)!=1)
        error = 1;
    else 
        if (arg==NULL)
            error = 2;
    else
        error = c_zero_int(arg);

    switch (error) {
        case 0:
            Py_DecRef(arg);
            Py_RETURN_NONE;
            break;
        case 1:
            message = (char *) malloc(sizeof("Argument isn't int type. ") + 1);
            strcpy(message, "Argument isn't int type. ");
            type = PyExc_ValueError;
            break;
        case 2:
            message = (char *) malloc(sizeof("Argument was NULL. ") + 1);
            strcpy(message, "Argument was NULL. ");
            type = PyExc_ValueError;
            break;
        case 3:
            message = (char *) malloc(sizeof("Argument was a preallocated integer between 0 & 256. ") + 1);
            strcpy(message, "Argument was a preallocated integer between 0 & 256. ");
            type = PyExc_ValueError;
            break;
        case 4:
            message = (char *) malloc(sizeof("Argument was a signed integer. ") + 1);
            strcpy(message, "Argument was a signed integer. ");
            type = PyExc_ValueError;
            break;
        default:
            message = (char *) malloc(sizeof("Unknown exception. ") + 1);
            strcpy(message, "Unknown exception. ");
            type = PyExc_UserWarning;
    }
    Py_DecRef(arg);
    PyErr_SetString(type, message);
    free(message);
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
