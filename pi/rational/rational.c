#include "Python.h"
#include "structmember.h"
#include "rational.h"

static void 
rational_dealloc(PyObject *op)
{
    op->ob_type->tp_free(op);
}

static PyObject *
rational_format(PyRationalObject *v, int precision, char format_code)
{
    PyObject *result = NULL;
    Py_ssize_t len;
    
    char *num = NULL:
    char *den = NULL;
    char *buf = NULL;
    
    num = PyOS_double_to_string(v->num, format_code, precision, 0, NULL);
    if (!num) {
        PyErr_NoMemory();
        goto done;
    }
    
    den = PyOS_double_to_string(v->den, format_code, precision, 0, NULL);
    if (!den) {
        PyErr_NoMemory();
        goto done;
    }
    
    len = strlen(num) + strlen(den) + 2;
    buf = PyMem_Malloc(len);
    if (!buf) {
        PyErr_NoMemory();
        goto done;
    }
    PyOS_snprintf(buf, len, "%s/%s", num, den);
    result = PyString_FromString(buf);
done:
    PyMem_Free(num);
    PyMem_Free(den);
    PyMem_Free(buf);
    
    return result;
}


PyTypeObject PyRational_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "rational"
    sizeof(PyRationalObject),
    0,
    rational_dealloc,                                   /* tp_dealloc */
    
    
}