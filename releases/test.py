# -*- coding: utf-8 -*-
from pi import rational
from pi import quaternion
from pi import stack

# test rational
r = rational(1, 2)
print r.__class__
print r.__doc__
print str(r)
print repr(r)
print hash(r)
print r.num
print r.den

r1 = rational(1, 2)
r2 = rational(3, 4)
print r1 + r2
print r1 - r2
print r1 * r2
print r1 / r2
print -r1
print +r1
print abs(r1)
print r1.__nonzero__()

# test quaternion
q = quaternion(1, 2, 3, 4)
print q.__class__
print q.__doc__
print str(q)
print repr(q)
print hash(q)
print q.conjugate()
print q.inverse()
print q.r
print q.i
print q.j
print q.k

q1 = quaternion(1, 1, 1, 1)
q2 = quaternion(2, 2, 2, 2)
print q1 + q2
print q1 - q2
print q1 * q2
print q1 / q2
print -q1
print +q1
print abs(q1)
print q1.__nonzero__()

# test stack
s = stack()
s.push(123)
s.push("123")
s.push([1, 2, 3])
s.push(123)
print len(s), s
print s.pop()
print len(s), s
print repr(s)
print str(s)
for i in s:
    print i
