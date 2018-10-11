#include "Python.h"
#include "structmember.h"
#include "quaternion.h"

static void 
quaternion_dealloc(PyObject *op) 
{
    op->ob_type->tp_free(op);
}

static PyObject* 
quaternion_format(PyQuaternionObject *v, int precision, char format_code)
{
    PyObject* result = NULL;
    Py_ssize_t len;
    
    char *r = NULL;
    char *i = NULL;
    char *j = NULL;
    char *k = NULL;
    char *buf = NULL;
    
    r = PyOS_double_to_string(v->r, format_code, precision, 0, NULL);
    if (!r) {
        PyErr_NoMemory();
        goto done;
    }
    
    i = PyOS_double_to_string(v->i, format_code, precision, Py_DTSF_SIGN, NULL);
    if (!i) {
        PyErr_NoMemory();
        goto done;
    }
    
    j = PyOS_double_to_string(v->j, format_code, precision, Py_DTSF_SIGN, NULL);
    if (!j) {
        PyErr_NoMemory();
        goto done;
    }
    
    k = PyOS_double_to_string(v->k, format_code, precision, Py_DTSF_SIGN, NULL);
    if (!k) {
        PyErr_NoMemory();
        goto done;
    }
    
    len = strlen(r) + strlen(i) + strlen(j) + strlen(k) + 14;
    buf = PyMem_Malloc(len);
    if (!buf) {
        PyErr_NoMemory();
        goto done;
    }
    PyOS_snprintf(buf, len, "(%s + %si + %sj + %sk)");
    result = PyString_FromString(buf);
    
done:
    PyMem_Free(r);
    PyMem_Free(i);
    PyMem_Free(j);
    PyMem_Free(k);
    
    return result;
}

static int 
quaternion_print(PyQuaternionObject *v, FILE *fp, int flags) 
{
    
    PyObject *formatv;
    char *buf;
    if (flags & Py_Print_RAW)
        formatv = quaternion_format(v, PyFloat_STR_PRECISION, 'g');
    else
        formatv = quaternion_format(v, 0, 'r');
    if (formatv == NULL)
        return -1;
    buf = PyString_AS_STRING(formatv);
    Py_BEGIN_ALLOW_THREADS
    fputs(buf, fp);
    Py_END_ALLOW_THREADS
    Py_DECREF(formatv);
    return 0;
}

static PyObject *
quaternion_repr(PyQuaternionObject *v) 
{
    return quaternion_format(v, 0, 'r');
}

static PyObject *
quaternion_str(PyQuaternionObject *v)
{
    return quaternion_format(v, PyFloat_STR_PRECISION, 'g');
}

PyTypeObject PyQuaternion_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "quaternion",
    sizeof(PyQuaternionObject),
    0,
    quaternion_dealloc,                                    /* tp_dealloc */
    (printfunc)quaternion_print,                           /* tp_print */
    0,                                                     /* tp_getattr */
    0,                                                     /* tp_setattr */
    0,                                                     /* tp_compare */
    (reprfunc)quaternion_repr                              /* tp_repr */
    
}