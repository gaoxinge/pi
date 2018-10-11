/* Rational number structure */

#ifndef Py_RATIONAL_H
#define Py_RATIONAL_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PyObject_HEAD
    double a;
    double b;
} PyRationalObject;

PyTypeObject PyRational_Type;

#define PyRational_Check(op)      PyObject_TypeCheck(op, &PyRational_Type)
#define PyRational_CheckExact(op) (Py_TYPE(op) == &PyRational_Type)

PyObject *PyRational_FromDoubles  (double num, double den);
double   *PyRational_NumAsDoubles (PyObject *op);
double   *PyRational_DenAsDoubles (PyObject *op);

#ifdef __cplusplus
}
#endif
#endif /* !Py_RATIONAL_H */