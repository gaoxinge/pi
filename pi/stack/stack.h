#ifndef Py_STACK_H
#define Py_STACK_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _elem{
    PyObject     *item;
    struct _elem *next;
} Elem;

typedef struct {
    PyObject_VAR_HEAD
    Elem *root;
} PyStackObject;

PyTypeObject PyStack_Type;

PyObject *PyStack_New    ();
void      PyStack_Push   (PyStackObject *, PyObject *);
PyObject *PyStack_Pop    (PyStackObject *);
PyObject *PyStack_ToList (PyStackObject *);

#ifdef __cplusplus
}
#endif
#endif /* !Py_STACK_H */