import sys
is_pkpy = not hasattr(sys, 'getrefcount')

if is_pkpy:
    import numpy_bindings as np
else:
    import numpy as np

import math

# test dtypes
assert hasattr(np, 'int8')
assert hasattr(np, 'int16')
assert hasattr(np, 'int32')
assert hasattr(np, 'int64')
assert hasattr(np, 'int_')
assert hasattr(np, 'float32')
assert hasattr(np, 'float64')
assert hasattr(np, 'float_')
assert hasattr(np, 'bool_')

# test ndarray + - / * ** @
a = np.ones([2, 3])
'''
array([[1., 1., 1.],
       [1., 1., 1.]])
'''

def assert_equal(a, b):
    assert (a == b).all()

assert_equal(a + 1, np.array([[2., 2., 2.], [2., 2., 2.]]))
assert_equal(a - 1, np.array([[0., 0., 0.], [0., 0., 0.]]))
assert_equal(a * 2, np.array([[2., 2., 2.], [2., 2., 2.]]))
assert_equal(a / 2, np.array([[0.5, 0.5, 0.5], [0.5, 0.5, 0.5]]))
assert_equal(a ** 2, np.array([[1., 1., 1.], [1., 1., 1.]]))
assert_equal(a @ np.ones([3, 4]), np.ones([2, 4]) * 3)

# test & | ^ ~
a = np.array([[1, 0], [0, 1]], dtype=np.bool_)
'''
array([[ True, False],
       [False,  True]])
'''
assert_equal(a & True, np.array([[1, 0], [0, 1]], dtype=np.bool_))
assert_equal(a | True, np.array([[1, 1], [1, 1]], dtype=np.bool_))
assert_equal(a ^ True, np.array([[0, 1], [1, 0]], dtype=np.bool_))
assert_equal(~a, np.array([[False,  True], [ True, False]]))

# test __getitem__ __setitem__
a = np.array([[1, 2], [3, 4]])
'''
array([[1, 2],
       [3, 4]])
'''
assert_equal(a[0], np.array([1, 2]))
assert a[0, 1] == 2
a[0, 1] = 3
assert a[0, 1] == 3

# test __len__
a = np.array([[1, 2], [3, 4]])
assert len(a) == 2
assert a.shape == (2, 2)

# test __repr__ __str__
a = np.array([[1, 2], [3, 4]])
assert repr(a) == '''
array([[1, 2],
       [3, 4]])
'''.strip()

# test dtype ndim size shape
a = np.array([[1, 2], [3, 4]])
assert (a.dtype == np.int64), a.dtype
assert a.ndim == 2
assert a.size == 4
assert a.shape == (2, 2)

# test all any
a = np.array([[True, False], [True, True]])
assert a.all() == False
assert a.any() == True

# test sum, min, max, mean, std, var
a = np.array([1.0, 2.0, 3.0])
assert math.isclose(a.sum(), 6.0)
assert math.isclose(a.min(), 1.0)
assert math.isclose(a.max(), 3.0)
assert math.isclose(a.mean(), 2.0)
assert math.isclose(a.std(), 0.816496580927726)
assert math.isclose(a.var(), 0.6666666666666666)

# test argmin, argmax, argsort, sort
a = np.array([3, 1, 2])
assert a.argmin() == 1
assert a.argmax() == 0
assert_equal(a.argsort(), np.array([1, 2, 0]))
a.sort()
assert_equal(a, np.array([1, 2, 3]))

# test reshape, resize, repeat, transpose, squeeze, flatten
a = np.array([[1, 2], [3, 4]])
assert_equal(a.reshape([1, 4]), np.array([[1, 2, 3, 4]]))
a.resize([1, 4])
assert_equal(a, np.array([[1, 2, 3, 4]]))
assert_equal(a.repeat(2), np.array([[1, 1, 2, 2, 3, 3, 4, 4]]))
assert_equal(a.transpose(), np.array([[1], [2], [3], [4]]))
assert_equal(a.squeeze(), np.array([1, 2, 3, 4]))
assert_equal(a.transpose().flatten(), np.array([1, 2, 3, 4]))

# test astype, round, copy, tolist
a = np.array([1.1, 2.2, 3.3])
assert_equal(a.astype(np.int32), np.array([1, 2, 3]))
assert_equal(a.round(), np.array([1., 2., 3.]))
assert_equal(a.copy(), a)
assert a.tolist() == [1.1, 2.2, 3.3]
