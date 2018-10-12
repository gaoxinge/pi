#include "Python.h"
#include "structmember.h"
#include "rational.h"

#define CONVERT_TO_DOUBLE(obj, num0, den0)       \
    if (PyRational_Check(obj)) {                 \
        num0 = ((PyRationalObject *)(obj))->num; \
        den0 = ((PyRationalObject *)(obj))->den; \
    } else {                                     \
        Py_INCREF(Py_NotImplemented);            \
        return Py_NotImplemented;                \
    }
    
#define ABS(obj) (((obj) > 0) ? (obj) : (-(obj)))

PyObject *
PyRational_FromDoubles(double num, double den)
{
    PyRationalObject *v = NULL;
    v = (PyRationalObject *) (&PyRational_Type)->tp_alloc(&PyRational_Type, 0);
    if (v == NULL)
        return NULL;
    
    v->num = num;
    v->den = den;
    
    return (PyObject *)v;
}

double
PyRational_NumAsDouble(PyObject *op) {
    if (PyRational_Check(op)) 
        return ((PyRationalObject *)op)->num;
    else
        return 0.0;
}

double
PyRational_DenAsDouble(PyObject *op) {
    if (PyRational_Check(op))
        return ((PyRationalObject *)op)->den;
    else
        return 0.0;
}

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
    
    char *num = NULL;
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
    
    len = strlen(num) + strlen(den) + 4;
    buf = PyMem_Malloc(len);
    if (!buf) {
        PyErr_NoMemory();
        goto done;
    }
    PyOS_snprintf(buf, len, "%s / %s", num, den);
    result = PyString_FromString(buf);

done:
    PyMem_Free(num);
    PyMem_Free(den);
    PyMem_Free(buf);
    
    return result;
}

static PyObject *
rational_repr(PyRationalObject *v)
{
    return rational_format(v, 0, 'r');
}

static PyObject *
rational_str(PyRationalObject *v)
{
    return rational_format(v, PyFloat_STR_PRECISION, 'g');
}

static PyObject *
rational_add(PyObject *v, PyObject *w) 
{
    double num0, den0;
    double num1, den1;
    double num2, den2;
    CONVERT_TO_DOUBLE(v, num1, den1);
    CONVERT_TO_DOUBLE(w, num2, den2);
    num0 = num1 * den2 + den1 * num2;
    den0 = den1 * den2;
    return PyRational_FromDoubles(num0, den0);
}

static PyObject *
rational_sub(PyObject *v, PyObject *w) 
{
    double num0, den0;
    double num1, den1;
    double num2, den2;
    CONVERT_TO_DOUBLE(v, num1, den1);
    CONVERT_TO_DOUBLE(w, num2, den2);
    num0 = num1 * den2 - den1 * num2;
    den0 = den1 * den2;
    return PyRational_FromDoubles(num0, den0);
}

static PyObject *
rational_mul(PyObject *v, PyObject *w) 
{
    double num0, den0;
    double num1, den1;
    double num2, den2;
    CONVERT_TO_DOUBLE(v, num1, den1);
    CONVERT_TO_DOUBLE(w, num2, den2);
    num0 = num1 * num2;
    den0 = den1 * den2;
    return PyRational_FromDoubles(num0, den0);
}

static PyObject *
rational_div(PyObject *v, PyObject *w)
{
    double num0, den0;
    double num1, den1;
    double num2, den2;
    CONVERT_TO_DOUBLE(v, num1, den1);
    CONVERT_TO_DOUBLE(w, num2, den2);
    num0 = num1 * num2;
    den0 = den1 * den2;
    return PyRational_FromDoubles(num0, den0);
}

static PyObject *
rational_neg(PyRationalObject *v)
{
    return PyRational_FromDoubles(-v->num, v->den);
}

static PyObject *
rational_pos(PyRationalObject *v) 
{
    if (PyRational_CheckExact(v)) {
        Py_INCREF(v);
        return (PyObject *)v;
    } else
        return PyRational_FromDoubles(v->num, v->den);
}

static PyObject *
rational_abs(PyRationalObject *v)
{
    return PyRational_FromDoubles(ABS(v->num), ABS(v->den));
}

static int
rational_nonzero(PyRationalObject *v)
{
    return v->num != 0;
}

static long
rational_hash(PyRationalObject *v)
{
     long hashnum, hashden, combined;
     hashnum = _Py_HashDouble(v->num);
     if (hashnum == -1)
         return -1;
     hashden = _Py_HashDouble(v->den);
     if (hashden == -1)
         return -1;
     combined = hashnum + 1000003 * hashden;
     if (combined == -1)
         return -2;
     return combined;
}

static PyNumberMethods rational_as_number = {
    (binaryfunc)rational_add,                           /* nb_add */
    (binaryfunc)rational_sub,                           /* nb_sub */
    (binaryfunc)rational_mul,                           /* nb_multiply */
    (binaryfunc)rational_div,                           /* nb_divide */
    0,                                                  /* nb_remainder */
    0,                                                  /* nb_divmod */
    0,                                                  /* nb_power */
    (unaryfunc)rational_neg,                            /* nb_neg */
    (unaryfunc)rational_pos,                            /* nb_pos */
    (unaryfunc)rational_abs,                            /* nb_absolute */
    (inquiry)rational_nonzero,                          /* nb_nonzero */
    0,                                                  /* nb_invert */
    0,                                                  /* nb_lshift */
    0,                                                  /* nb_rshitf */
    0,                                                  /* nb_and */
    0,                                                  /* nb_xor */
    0,                                                  /* nb_or */
    0,                                                  /* nb_coerce */
    0,                                                  /* nb_int */
    0,                                                  /* nb_long */
    0,                                                  /* nb_float */
    0,                                                  /* nb_oct */
    0,                                                  /* nb_hex */
    0,                                                  /* nb_inplace_add */
    0,                                                  /* nb_inplace_subtract */
    0,                                                  /* nb_inplace_multiply */
    0,                                                  /* nb_inplace_divide */
    0,                                                  /* nb_inplace_remainder */
    0,                                                  /* nb_inplace_power */
    0,                                                  /* nb_inplace_lshift */
    0,                                                  /* nb_inplace_rshift */
    0,                                                  /* nb_inplace_and */
    0,                                                  /* nb_inplace_xor */
    0,                                                  /* nb_inplace_or */
    0,                                                  /* nb_floor_divide */
    0,                                                  /* nb_true_divide */
    0,                                                  /* nb_inplace_floor_divide */
    0,                                                  /* nb_inplace_true_divide */
};

static PyMemberDef rational_members[] = {
    {"num", T_DOUBLE, offsetof(PyRationalObject, num), READONLY, 
     "the num part of rational numbeer"},
    {"den", T_DOUBLE, offsetof(PyRationalObject, den), READONLY,
     "the den part of rational number"},
    {NULL},
};

static PyObject *
rational_new(PyTypeObject *type, PyObject *args, PyObject *kwargs) 
{
    PyRationalObject *v = NULL;
    v = (PyRationalObject *) type->tp_alloc(type, 0);
    if (v == NULL)
        return NULL;
    
    static char *kwlist[] = {"num", "den", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|dd", kwlist, &v->num, &v->den))
        return NULL;
    
    return (PyObject *)v;
}

PyTypeObject PyRational_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "rational",
    sizeof(PyRationalObject),
    0,
    (destructor)rational_dealloc,                       /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_compare */
    (reprfunc)rational_repr,                            /* tp_repr */
    &rational_as_number,                                /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    (hashfunc)rational_hash,                            /* tp_hash */
    0,                                                  /* tp_call */
    (reprfunc)rational_str,                             /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES |
    Py_TPFLAGS_BASETYPE,                                /* tp_flags */
    "rational(num, den) -> rational number",            /* tp_doc */
    0,                                                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    0,                                                  /* tp_iter */
    0,                                                  /* tp_iternext */
    0,                                                  /* tp_methods */
    rational_members,                                   /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    rational_new,                                       /* tp_new */
};

static PyMethodDef RationalMethods[] = {
    {NULL},
};

void 
initrational()
{
    PyObject* m = NULL;
    m = Py_InitModule("rational", RationalMethods);
    if (m == NULL)
        return;
    
    if (PyType_Ready(&PyRational_Type) < 0)
        return;
    Py_INCREF(&PyRational_Type);
    PyModule_AddObject(m, "rational", (PyObject *) &PyRational_Type);
}