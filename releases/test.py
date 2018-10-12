# -*- coding: utf-8 -*-
from pi import rational

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