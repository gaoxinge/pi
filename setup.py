# -*- coding: utf-8 -*-
from distutils.core import setup
from distutils.core import Extension

rational = Extension(
    name="pi.rational",
    sources=["pi/rational/rational.c"],
    depends=["pi/rational/rational.h"],
)

quaternion = Extension(
    name="pi.quaternion",
    sources=["pi/quaternion/quaternion.c"],
    depends=["pi/quaternion/quaternion.h"],
)

stack = Extension(
    name="pi.stack",
    sources=["pi/stack/stack.c"],
    depends=["pi/stack/stack.h"],
)

setup(
    name="pi", 
    version="1.0.0",
    packages=["pi"],
    ext_modules=[rational, quaternion, stack],
)