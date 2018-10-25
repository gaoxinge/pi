#include "Python.h"
#include "structmember.h"
#include "queue.h"

PyObject *
PyQueue_New() {
    PyQueueObject *v = PyObject_GC_New(PyQueueObject, &PyQueue_Type);
    if (v == NULL)
        return NULL;
    v->root = NULL;
    v->tail = NULL;
    v->ob_size = 0;
    return (PyObject *) v;
}

void 
PyQueue_Add(PyQueueObject *v, PyObject *ob) {
    Elems *tmp = PyMem_MALLOC(sizeof(Elems));
    Py_XINCREF(ob);
    tmp->item = ob;
    if (v->root != NULL) Py_XDECREF(v->root->item);
    tmp->next = v->root;
    tmp->last = NULL;
    
    if (v->root != NULL) {
        v->root->last = tmp;
        Py_XINCREF(v->root->last->item);
    }
    
    v->root = tmp;
    v->ob_size++;
    Py_XINCREF(v->root->item);
    
    if (v->tail == NULL) {
        v->tail = tmp;
        Py_XINCREF(v->tail->item);
    }
}

PyObject *
PyQueue_Delete(PyQueueObject *v) {
    Elems *tmp = v->tail;
    PyObject *ob = tmp->item;
    v->tail = tmp->last;
    v->tail->next = NULL;
    if (v->tail == NULL) {
        v->root = NULL;
    }
    v->ob_size--;
    Py_XDECREF(ob);
    PyMem_FREE(tmp);
    return ob;
}

PyObject *
PyQueue_ToList(PyQueueObject *v) {
    PyObject *l = PyList_New(v->ob_size);
    Elems *tmp = v->root;
    Py_ssize_t i = v->ob_size - 1;
    while (tmp) {
        PyList_SetItem(l, i, tmp->item);
        Py_XINCREF(tmp->item);
        tmp = tmp->next;
        i -= 1;
    }
    return l;
}

static void 
queue_dealloc(PyQueueObject *v) {
    Elems *tmp = v->root;
    Elems *ttmp;
    while (tmp) {
        ttmp = tmp->next;
        Py_XDECREF(tmp->item);
        PyMem_FREE(tmp);
        tmp = ttmp;
    }
    v->ob_type->tp_free(v);
}

static PyObject *
queue_repr(PyQueueObject *v) {
    PyObject *l = PyQueue_ToList(v);
    PyObject *s = PyObject_Str(l);
    Py_XDECREF(l);
    return s;
}

static Py_ssize_t
queue_length(PyQueueObject *v) {
    return v->ob_size;
}

static PySequenceMethods queue_as_sequence = {
    (lenfunc)queue_length,             /* sq_length */
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

static PyMappingMethods queue_as_mapping = {
    (lenfunc)queue_length,             /* ma_length */
    0,                                 /* ma_subscript */
    0,                                 /* ma_ass_subscript */
};

static PyObject *
queue_str(PyQueueObject *v) {
    PyObject *l = PyQueue_ToList(v);
    PyObject *s = PyObject_Str(l);
    Py_XDECREF(l);
    return s;
}

static PyObject *
queue_iter(PyQueueObject *v) {
    PyObject *l = PyQueue_ToList(v);
    PyObject *i = (&PyList_Type)->tp_iter(l);
    Py_XDECREF(l);
    return i;
}

static PyMethodDef queue_methods[] = {
    {"add", (PyCFunction)PyQueue_Add, METH_O,
     "queue.add(x)"},
    {"delete", (PyCFunction)PyQueue_Delete, METH_NOARGS,
     "queue.delete() -> x"},
    {NULL},
};

static PyObject *
queue_new(PyTypeObject *type, PyObject *args, PyObject *kw) {
    return PyQueue_New();
}

PyTypeObject PyQueue_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "queue",
    sizeof(PyQueueObject),
    0,
    (destructor)queue_dealloc,                          /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_compare */
    (reprfunc)queue_repr,                               /* tp_repr */
    0,                                                  /* tp_as_number */
    &queue_as_sequence,                                 /* tp_as_sequence */
    &queue_as_mapping,                                  /* tp_as_mapping */
    (hashfunc)PyObject_HashNotImplemented,              /* tp_hash */
    0,                                                  /* tp_call */
    (reprfunc)queue_str,                                /* tp_str */
    PyObject_GenericGetAttr,                            /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_CHECKTYPES |
    Py_TPFLAGS_BASETYPE,                                /* tp_flags */
    "queue() -> new empty queue\n",                     /* tp_doc */
    0,                                                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    (getiterfunc)queue_iter,                            /* tp_iter */
    0,                                                  /* tp_iternext */
    queue_methods,                                      /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    queue_new,                                          /* tp_new */
    PyObject_GC_Del,                                    /* tp_free */
};

static PyMethodDef QueueMethods[] = {
    {NULL},
};

void 
initqueue()
{
    PyObject* m = NULL;
    m = Py_InitModule("queue", QueueMethods);
    if (m == NULL)
        return;
    
    if (PyType_Ready(&PyQueue_Type) < 0)
        return;
    Py_INCREF(&PyQueue_Type);
    PyModule_AddObject(m, "queue", (PyObject *) &PyQueue_Type);
}