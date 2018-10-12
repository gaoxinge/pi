#ifndef Py_QUATERNION_H
#define Py_QUATERNION_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PyObject_HEAD
    double r;
    double i;
    double j;
    double k;
} PyQuaternionObject;

PyTypeObject PyQuaternion_Type;

#define PyQuaternion_Check(op)      PyObject_TypeCheck(op, &PyQuaternion_Type)
#define PyQuaternion_CheckExact(op) (Py_TYPE(op) == &PyQuaternion_Type)

PyObject *PyQuaternion_FromDoubles (double r, double i, double j, double k);
double    PyQuaternion_RAsDouble   (PyObject *op);
double    PyQuaternion_IAsDouble   (PyObject *op);
double    PyQuaternion_JAsDouble   (PyObject *op);
double    PyQuaternion_KAsDouble   (PyObject *op);

#ifdef __cplusplus
}
#endif
#endif /* !Py_QUATERNION_H */