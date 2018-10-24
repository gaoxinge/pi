#include "Python.h"
#include "structmember.h"
#include "stack.h"

PyObject *
PyStack_New() {
    PyStackObject *v = PyObject_GC_New(PyStackObject, &PyStack_Type);
    if (v == NULL)
        return NULL;
    v->root = NULL;
    v->ob_size = 0;
    return (PyObject *) v;
}

void 
PyStack_Push(PyStackObject *v, PyObject *ob) {
    Elem *tmp = PyMem_Malloc(sizeof(Elem));
    tmp->item = ob;
    tmp->next = v->root;
    v->root = tmp;
    v->ob_size++;
    Py_XINCREF(ob);
    Py_XINCREF(v->root->item);
}

PyObject *
PyStack_Pop(PyStackObject *v) {
    Elem *tmp = v->root;
    PyObject *ob = v->root->item;
    v->root = tmp->next;
    v->ob_size--;
    PyMem_Free(tmp);
    Py_XDECREF(ob);
    return ob;
}

PyObject *
PyStack_ToList(PyStackObject *v) {
    PyObject *l = PyList_New(v->ob_size);
    Elem *tmp = v->root;
    Py_ssize_t i = v->ob_size - 1;
    while (tmp) {
        PyList_SetItem(l, i, tmp->item);
        tmp = tmp->next;
        i -= 1;
    }
    return l;
}

static void 
stack_dealloc(PyStackObject *v) {
    Elem *tmp = v->root;
    Elem *ttmp;
    while (tmp) {
        ttmp = tmp->next;
        Py_XDECREF(tmp->item);
        PyMem_FREE(tmp);
        tmp = ttmp;
    }
    v->ob_type->tp_free(v);
}

static PyObject *
stack_repr(PyStackObject *v) {
    return PyObject_Repr(PyStack_ToList(v));
}

static Py_ssize_t
stack_length(PyStackObject *v) {
    return v->ob_size;
}

static PySequenceMethods stack_as_sequence = {
    (lenfunc)stack_length,             /* sq_length */
    0,                                 /* sq_concat */
    0,                                 /* sq_repeat */
    0,                                 /* sq_item */
    0,                                 /* sq_slice */
    0,                                 /* sq_ass_item */
    0,                                 /* sq_ass_slice */
    0,                                 /* sq_contains */
    0,                                 /* sq_inplace_concat */
    0,                                 /* sq_inplace_repeat */
};

static PyMappingMethods stack_as_mapping = {
    (lenfunc)stack_length,             /* ma_length */
    0,                                 /* ma_subscript */
    0,                                 /* ma_ass_subscript */
};

static PyObject *
stack_str(PyStackObject *v) {
    return PyObject_Str(PyStack_ToList(v));
}

static PyObject *
stack_iter(PyStackObject *op) {
    return (&PyList_Type)->tp_iter(PyStack_ToList(op));
}

static PyMethodDef stack_methods[] = {
    {"push", (PyCFunction)PyStack_Push, METH_O,
     "stack.push(x)"},
    {"pop", (PyCFunction)PyStack_Pop, METH_NOARGS,
     "stack.pop() -> x"},
    {NULL},
};

static PyObject *
stack_new(PyTypeObject *type, PyObject *args, PyObject *kw) {
    return PyStack_New();
}

PyTypeObject PyStack_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "stack",
    sizeof(PyStackObject),
    0,
    (destructor)stack_dealloc,                          /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_compare */
    (reprfunc)stack_repr,                               /* tp_repr */
    0,                                                  /* tp_as_number */
    &stack_as_sequence,                                 /* tp_as_sequence */
    &stack_as_mapping,                                  /* tp_as_mapping */
    (hashfunc)PyObject_HashNotImplemented,              /* tp_hash */
    0,                                                  /* tp_call */
    (reprfunc)stack_str,                                /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES |
    Py_TPFLAGS_BASETYPE,                                /* tp_flags */
    "stack() -> new empty stack\n",                     /* tp_doc */
    0,                                                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    (getiterfunc)stack_iter,                            /* tp_iter */
    0,                                                  /* tp_iternext */
    stack_methods,                                      /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    stack_new,                                          /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};

static PyMethodDef StackMethods[] = {
    {NULL},
};

void 
initstack()
{
    PyObject* m = NULL;
    m = Py_InitModule("stack", StackMethods);
    if (m == NULL)
        return;
    
    if (PyType_Ready(&PyStack_Type) < 0)
        return;
    Py_INCREF(&PyStack_Type);
    PyModule_AddObject(m, "stack", (PyObject *) &PyStack_Type);
}