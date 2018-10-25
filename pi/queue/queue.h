#ifndef Py_QUEUE_H
#define Py_QUEUE_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _elems {
    PyObject      *item;
    struct _elems *last;
    struct _elems *next; 
} Elems;

typedef struct {
    PyObject_VAR_HEAD
    Elems *root;
    Elems *tail;
} PyQueueObject;

PyTypeObject PyQueue_Type;

PyObject *PyQueue_New    ();
void      PyQueue_Add    (PyQueueObject *, PyObject *);
PyObject *PyQueue_Delete (PyQueueObject *);
PyObject *PyQueue_ToList (PyQueueObject *);

#ifdef __cplusplus
}
#endif
#endif /* Py_QUEUE_H */