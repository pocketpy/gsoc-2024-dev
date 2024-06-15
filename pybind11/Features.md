This is a feature list of pybind11 for pocketpy. It lists all completed and pending features. It also lists the features that cannot be implemented in the current version of pocketpy.

## [Function](https://pybind11.readthedocs.io/en/stable/advanced/functions.html)

- [x] Function overloading
- [x] Return value policy
- [x] is_prepend
- [x] `*args` and `**kwargs`
- [ ] Keep-alive
- [ ] Call Guard
- [x] Default arguments
- [ ] Keyword-Only arguments
- [ ] Positional-Only arguments
- [ ] Allow/Prohibiting None arguments

## [Class](https://pybind11.readthedocs.io/en/stable/classes.html)

- [x] Creating bindings for a custom type
- [x] Binding lambda functions
- [ ] Dynamic attributes
- [x] Inheritance and automatic downcasting
- [x] Enumerations and internal types
- [ ] Instance and static fields

> Binding static fields may never be implemented in pocketpy because it requires a metaclass, which is a heavy and infrequently used feature.

## [Exceptions](https://pybind11.readthedocs.io/en/stable/advanced/exceptions.html)

Need further discussion.

## [Smart pointers](https://pybind11.readthedocs.io/en/stable/advanced/smart_ptrs.html)

- [ ] std::shared_ptr
- [ ] std::unique_ptr
- [ ] Custom smart pointers

## [Type conversions](https://pybind11.readthedocs.io/en/stable/advanced/cast/index.html)

- [x] Python built-in types
- [x] STL Containers
- [ ] Functional
- [ ] Chrono

## [Python C++ interface](https://pybind11.readthedocs.io/en/stable/advanced/pycpp/object.html)

Need further discussion.

- [x] `object`
- [x] `none`
- [x] `type`
- [x] `bool_`
- [x] `int_`
- [x] `float_`
- [x] `str`
- [ ] `bytes`
- [ ] `bytearray`
- [x] `tuple`
- [x] `list`
- [ ] `set`
- [x] `dict`
- [ ] `slice`
- [x] `iterable`
- [x] `iterator`
- [ ] `function`
- [ ] `buffer`
- [ ] `memoryview`
- [x] `capsule`

## [Miscellaneous](https://pybind11.readthedocs.io/en/stable/advanced/misc.html)

- [ ] Global Interpreter Lock (GIL)
- [ ] Binding sequence data types, iterators, the slicing protocol, etc.
- [x] Convenient operators binding

# Differences between CPython and pocketpy

- only `add`, `sub` and `mul` have corresponding right versions in pocketpy. So if you bind `int() >> py::self`, it will has no effect in pocketpy.

- in-place operators, such as `+=`, `-=`, `*=`, etc., are not supported in pocketpy.

- thre return value of `globals` is immutable in pocketpy.
