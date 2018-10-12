#include "Python.h"
#include "structmember.h"
#include "quaternion.h"

#define CONVERT_TO_DOUBLE(obj, r0, i0, j0, k0) \
    if (PyQuaternion_Check(obj)) {             \
        r0 = ((PyQuaternionObject *)(obj))->r; \
        i0 = ((PyQuaternionObject *)(obj))->i; \
        j0 = ((PyQuaternionObject *)(obj))->j; \
        k0 = ((PyQuaternionObject *)(obj))->k; \
    } else {                                   \
        Py_INCREF(Py_NotImplemented);          \
        return Py_NotImplemented;              \
    }
    
static PyObject *quaternion_inverse(PyObject *op);

PyObject *
PyQuaternion_FromDoubles(double r, double i, double j, double k)
{
    PyQuaternionObject *v = NULL;
    v = (PyQuaternionObject *) (&PyQuaternion_Type)->tp_alloc(&PyQuaternion_Type, 0);
    if (v == NULL)
        return NULL;
    
    v->r = r;
    v->i = i;
    v->j = j;
    v->k = k;
    
    return (PyObject *) v;
}

double
PyQuaternion_RAsDoubles(PyObject *op)
{
    if (PyQuaternion_Check(op))
        return ((PyQuaternionObject *)op)->r;
    else
        return 0.0;
}

double
PyQuaternion_IAsDoubles(PyObject *op)
{
    if (PyQuaternion_Check(op))
        return ((PyQuaternionObject *)op)->i;
    else
        return 0.0;
}

double
PyQuaternion_JAsDoubles(PyObject *op)
{
    if (PyQuaternion_Check(op))
        return ((PyQuaternionObject *)op)->j;
    else
        return 0.0;
}

double
PyQuaternion_KAsDoubles(PyObject *op)
{
    if (PyQuaternion_Check(op))
        return ((PyQuaternionObject *)op)->k;
    else
        return 0.0;
}

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
    len = strlen(r) + strlen(i) + strlen(j) + strlen(k) + 4;
    buf = PyMem_Malloc(len);
    if (!buf) {
        PyErr_NoMemory();
        goto done;
    }
    PyOS_snprintf(buf, len, "%s%si%sj%sk", r, i, j, k);
    result = PyString_FromString(buf);
    
done:
    PyMem_Free(r);
    PyMem_Free(i);
    PyMem_Free(j);
    PyMem_Free(k);
    
    return result;
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

static PyObject *
quaternion_add(PyObject *v, PyObject *w)
{
    double r0, i0, j0, k0;
    double r1, i1, j1, k1;
    double r2, i2, j2, k2;
    CONVERT_TO_DOUBLE(v, r1, i1, j1, k1);
    CONVERT_TO_DOUBLE(w, r2, i2, j2, k2);
    r0 = r1 + r2;
    i0 = i1 + i2;
    j0 = j1 + j2;
    k0 = k1 + k2;
    return PyQuaternion_FromDoubles(r0, i0, j0, k0);
}

static PyObject *
quaternion_sub(PyObject *v, PyObject *w)
{
    double r0, i0, j0, k0;
    double r1, i1, j1, k1;
    double r2, i2, j2, k2;
    CONVERT_TO_DOUBLE(v, r1, i1, j1, k1);
    CONVERT_TO_DOUBLE(w, r2, i2, j2, k2);
    r0 = r1 - r2;
    i0 = i1 - i2;
    j0 = j1 - j2;
    k0 = k1 - k2;
    return PyQuaternion_FromDoubles(r0, i0, j0, k0);
}

static PyObject *
quaternion_mul(PyObject *v, PyObject *w)
{
    double r0, i0, j0, k0;
    double r1, i1, j1, k1;
    double r2, i2, j2, k2;
    CONVERT_TO_DOUBLE(v, r1, i1, j1, k1);
    CONVERT_TO_DOUBLE(w, r2, i2, j2, k2);
    r0 = r1 * r2 - i1 * i2 - j1 * j2 - k1 * k2;
    i0 = r1 * i2 + i1 * r2 + j1 * k2 - k1 * j2;
    j0 = r1 * j2 - i1 * k2 + j1 * r2 + k1 * i2;
    k0 = r1 * k2 + i1 * j2 - j1 * i2 + k1 * r2;
    return PyQuaternion_FromDoubles(r0, i0, j0, k0);
}

static PyObject *
quaternion_div(PyObject *v, PyObject *w)
{
    return quaternion_mul(v, quaternion_inverse(w));
}

static PyObject *
quaternion_neg(PyQuaternionObject *v)
{
    return PyQuaternion_FromDoubles(-v->r, -v->i, -v->j, -v->k);
}

static PyObject *
quaternion_pos(PyQuaternionObject *v)
{
    if (PyQuaternion_CheckExact(v)) {
        Py_INCREF(v);
        return (PyObject *) v;
    } else
        return PyQuaternion_FromDoubles(v->r, v->i, v->j, v->k);
}

static PyObject *
quaternion_abs(PyQuaternionObject *v)
{
    double s = sqrt(v->r * v->r + v->i * v->i + v->j * v->j + v->k * v->k);
    return PyFloat_FromDouble(s);
}

static int
quaternion_nonzero(PyQuaternionObject *v)
{
    return v->r != 0 || v->i != 0 || v->j != 0 || v->k != 0;
}

static PyNumberMethods quaternion_as_number = {
    (binaryfunc)quaternion_add,                         /* nb_add */
    (binaryfunc)quaternion_sub,                         /* nb_sub */
    (binaryfunc)quaternion_mul,                         /* nb_multiply */
    (binaryfunc)quaternion_div,                         /* nb_divide */
    0,                                                  /* nb_remainder */
    0,                                                  /* nb_divmod */
    0,                                                  /* nb_power */
    (unaryfunc)quaternion_neg,                          /* nb_neg */
    (unaryfunc)quaternion_pos,                          /* nb_pos */
    (unaryfunc)quaternion_abs,                          /* nb_absolute */
    (inquiry)quaternion_nonzero,                        /* nb_nonzero */
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

static long
quaternion_hash(PyQuaternionObject *v)
{
     long hashr, hashi, hashj, hashk;
     hashr = _Py_HashDouble(v->r);
     hashi = _Py_HashDouble(v->i);
     hashj = _Py_HashDouble(v->j);
     hashk = _Py_HashDouble(v->k);

     long combined = 0;
     combined = 1000003 * combined + hashk;
     combined = 1000003 * combined + hashj;
     combined = 1000003 * combined + hashi;
     combined = 1000003 * combined + hashr;
     return combined;
}

static PyObject *
quaternion_conjugate(PyObject *op) 
{   
    double r, i, j, k;
    CONVERT_TO_DOUBLE(op, r, i, j, k);
    return PyQuaternion_FromDoubles(r, -i, -j, -k);
}

static PyObject *
quaternion_inverse(PyObject *op)
{   
    double r, i, j, k;
    CONVERT_TO_DOUBLE(op, r, i, j, k);
    double ss = r * r + i * i + j * j + k * k;
    return PyQuaternion_FromDoubles(r / ss, -i / ss, -j / ss, -k / ss);
}

static PyMethodDef quaternion_methods[] = {
    {"conjugate", (PyCFunction)quaternion_conjugate, METH_NOARGS,
     "quaternion.conjugate() -> quaterion"},
    {"inverse", (PyCFunction)quaternion_inverse, METH_NOARGS,
     "quaternion.inverse() -> quaterion"},
    {NULL},
};

static PyMemberDef quaternion_members[] = {
    {"r", T_DOUBLE, offsetof(PyQuaternionObject, r), READONLY,
     "the r part of quaternion number"},
    {"i", T_DOUBLE, offsetof(PyQuaternionObject, i), READONLY,
     "the i part of quaternion number"},
    {"j", T_DOUBLE, offsetof(PyQuaternionObject, j), READONLY,
     "the j part of quaternion number"},
    {"k", T_DOUBLE, offsetof(PyQuaternionObject, k), READONLY,
     "the k part of quaternion number"},
    {NULL},
};

static PyObject *
quaternion_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    PyQuaternionObject *v = NULL;
    v = (PyQuaternionObject *) type->tp_alloc(type, 0);
    if (v == NULL)
        return NULL;
    
    static char *kwlist[] = {"r", "i", "j", "k", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|dddd", kwlist, &v->r, &v->i, &v->j, &v->k))
        return NULL;
    
    return (PyObject *)v;
}

PyTypeObject PyQuaternion_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "quaternion",
    sizeof(PyQuaternionObject),
    0,
    (destructor)quaternion_dealloc,                     /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_compare */
    (reprfunc)quaternion_repr,                          /* tp_repr */
    &quaternion_as_number,                              /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    (hashfunc)quaternion_hash,                          /* tp_hash */
    0,                                                  /* tp_call */
    (reprfunc)quaternion_str,                           /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES |
    Py_TPFLAGS_BASETYPE,                                /* tp_flags */
    "quaterion(r, i, j, k) -> quaterion number",        /* tp_doc */
    0,                                                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    0,                                                  /* tp_iter */
    0,                                                  /* tp_iternext */
    quaternion_methods,                                 /* tp_methods */
    quaternion_members,                                 /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    0,                                                  /* tp_alloc */
    quaternion_new,                                     /* tp_new */
};

static PyMethodDef QuaternionMethods[] = {
    {NULL},
};

void 
initquaternion()
{
    PyObject* m = NULL;
    m = Py_InitModule("quaternion", QuaternionMethods);
    if (m == NULL)
        return;
    
    if (PyType_Ready(&PyQuaternion_Type) < 0)
        return;
    Py_INCREF(&PyQuaternion_Type);
    PyModule_AddObject(m, "quaternion", (PyObject *) &PyQuaternion_Type);
}