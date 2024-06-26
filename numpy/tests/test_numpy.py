import pytest
import numpy_bindings as np


def test_array_int():
    arr1 = np.array([])
    arr2 = np.array(10)
    arr3 = np.array([-2, -1, 0, 1, 2])
    arr4 = np.array([[1, 2], [2, 1]])
    arr5 = np.array([[[1, 2, 3], [4, 5, 6]], [[7, 8, 9], [10, 11, 12]]])
    arr6 = np.array([[[[[1],[10], [100], [1000], [10000]]]]])
    arr7 = np.array([[[2147483647]]])


def test_array_bool():
    arr1 = np.array([True, False, True, False])
    arr2 = np.array([[True, False], [False, True]])
    arr3 = np.array([[[True, False, True], [False, True, False]], [[True, False, True], [False, True, False]]])


def test_array_float():
    arr1 = np.array([0.123456789])
    arr2 = np.array([-2.0, -1.0, 0.0, 1.0, 2.0])
    arr3 = np.array([[1.0, 2.0], [2.0, 1.0]])
    arr4 = np.array([[[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]], [[7.0, 8.0, 9.0], [10.0, 11.0, 12.0]]])
    arr5 = np.array([[[[[1.0],[10.0], [100.0], [1000.0], [10000.0]]]]])
    arr6 = np.array([[[3.141592653589793]]])


def test_array_creation():
    assert np.ones([1, 1]) == np.array([[1.0]])
    assert np.ones([1, 1, 2, 2]) == np.array([[[[1.0, 1.0], [1.0, 1.0]]]])

    assert np.zeros([1, 1]) == np.array([[0.0]])
    assert np.zeros([1, 1, 2, 2]) == np.array([[[[0.0, 0.0], [0.0, 0.0]]]])

    assert np.full([1, 1], -1e9) == np.array([[-1.000000e+09]])
    assert np.full([1, 1, 2, 2], 3.14) == np.array([[[[3.14, 3.14], [3.14, 3.14]]]])

    assert np.identity(1) == np.array([[1.0]])
    assert np.identity(3) == np.array([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])

    assert np.arange(10) == np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    assert np.arange(1, 10) == np.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
    assert np.arange(1, 10, 2) == np.array([1, 3, 5, 7, 9])

    assert np.linspace(0, 1, 5) == np.array([0.0, 0.25, 0.5, 0.75, 1.0])
    assert np.allclose(np.linspace(0, 1, 5, False), np.array([0.0, 0.2, 0.4, 0.6, 0.8]))
    assert np.allclose(np.linspace(0, 1, 20, True), np.array([0.0, 0.05263158, 0.10526316, 
                                                    0.15789474, 0.21052632, 0.26315789, 
                                                    0.31578947, 0.36842105, 0.42105263, 
                                                    0.47368421, 0.52631579, 0.57894737, 
                                                    0.63157895, 0.68421053, 0.73684211, 
                                                    0.78947368, 0.84210526, 0.89473684, 0.94736842, 1.0]))


def test_array_trigonometry():
    arr1 = np.array([np.pi/6, np.pi/4, np.pi/3, np.pi/2, np.pi])
    assert np.allclose(np.sin(arr1), np.array([0.5, 0.707107, 0.866025, 1.0, 0.0]))
    assert np.allclose(np.cos(arr1), np.array([0.866025, 0.707107, 0.5, 0.0, -1.0]))
    assert np.allclose(np.tan(arr1), np.array([0.57735, 1.0, 1.73205, np.inf , 0.0]))

    arr2 = np.array([0.5, 0.707107, 0.866025, 1.0, 0.0])
    assert np.allclose(np.arcsin(arr2), np.array([np.pi/6, np.pi/4, np.pi/3, np.pi/2, 0.0]))
    assert np.allclose(np.arccos(arr2), np.array([np.pi/3, np.pi/4, np.pi/6, 0.0, np.pi/2]))
    assert np.allclose(np.arctan(arr2), np.array([0.463648,  0.61548 ,  0.713724,  0.785398,  0.0]))


def test_array_properties():
    arr1 = np.array([1, 2, 3])
    assert arr1.size() == 3
    assert arr1.ndim() == 1
    assert arr1.shape() == [3]
    assert arr1.dtype() == 'int32'

    arr2 = np.array([[1, 2, 2, 1], [3, 4, 4, 3], [5, 6, 6, 5]])
    assert arr2.size() == 12
    assert arr2.ndim() == 2
    assert arr2.shape() == [3, 4]
    assert arr2.dtype() == 'int32'

    arr3 = np.array([[[[[1.5, 2.5], [3.5, 4.5], [5.5, 6.5]]]]])
    assert arr3.size() == 6
    assert arr3.ndim() == 5
    assert arr3.shape() == [1, 1, 1, 3, 2]
    assert arr3.dtype() == 'float64'


def test_boolean_functions():
    arr1 = np.array([0.0])
    assert arr1.all() == False
    assert arr1.any() == False

    arr2 = np.array([1.0])
    assert arr2.all() == True
    assert arr2.any() == True

    arr3 = np.array([[1, 0], [0, 1]])
    assert arr3.all() == False
    assert arr3.any() == True

    arr4 = np.array([[[True, False, True], [False, True, False]], [[True, False, True], [False, True, False]]])
    assert arr4.all() == False
    assert arr4.any() == True


def test_array_sum():
    arr1 = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
    assert arr1.sum() == 45
    assert arr1.sum(0) == np.array(45)

    arr2 = np.array([[1], [2], [3]])
    assert arr2.sum() == 6
    assert arr2.sum(0) == np.array([6])
    assert arr2.sum(1) == np.array([1, 2, 3])
    assert arr2.sum((0,1)) == np.array(6)

    arr3 = np.array([[[[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]]]])
    assert arr3.sum() == 40.5
    assert arr3.sum(0) == np.array([[[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]]])
    assert arr3.sum(1) == np.array([[[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]]])
    assert arr3.sum(2) == np.array([[[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]]])
    assert arr3.sum(3) == np.array([[[[10.5, 13.5, 16.5]]]])
    assert arr3.sum(4) == np.array([[[[7.5, 13.5, 19.5]]]])
    assert arr3.sum((0, 1)) == np.array([[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]])
    assert arr3.sum((3, 4)) == np.array([[[40.5]]])


def test_array_prod():
    arr1 = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
    assert arr1.prod() == 362880
    assert arr1.prod(0) == np.array(362880)

    arr2 = np.array([[1], [2], [3]])
    assert arr2.prod() == 6
    assert arr2.prod(0) == np.array([6])
    assert arr2.prod(1) == np.array([1, 2, 3])
    assert arr2.prod((0,1)) == np.array(6)

    arr3 = np.array([[[[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]]]])
    assert arr3.prod() == 304845.556640625
    assert arr3.prod(0) == np.array([[[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]]])
    assert arr3.prod(1) == np.array([[[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]]])
    assert arr3.prod(4) == np.array([[[[13.125,   86.625,  268.125]]]])
    assert arr3.prod((0, 3)) == np.array([[[28.875,   73.125,  144.375]]])


def test_array_min():
    arr1 = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
    assert arr1.min() == 1

    arr2 = np.array([[-1], [-2], [-3]])
    assert arr2.min() == -3

    arr3 = np.array([[[[[1.5, -1.5], [3.5, -3.5], [5.5, -5.5]]]]])
    assert arr3.min() == -5.5
    assert arr3.min(0) == np.array([[[[1.5, -1.5], [3.5, -3.5], [5.5, -5.5]]]])
    assert arr3.min(4) == np.array([[[[-1.5, -3.5, -5.5]]]])
    assert arr3.min((0, 3)) == np.array([[[1.5, -5.5]]])
    assert arr3.min((0, 4)) == np.array([[[-1.5, -3.5, -5.5]]])


def test_array_max():
    arr1 = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
    assert arr1.max() == 9

    arr2 = np.array([[-1], [-2], [-3]])
    assert arr2.max() == -1

    arr3 = np.array([[[[[1.5, -1.5], [3.5, -3.5], [5.5, -5.5]]]]])
    assert arr3.max() == 5.5
    assert arr3.max(0) == np.array([[[[1.5, -1.5], [3.5, -3.5], [5.5, -5.5]]]])
    assert arr3.max(4) == np.array([[[[1.5, 3.5, 5.5]]]])
    assert arr3.max((0, 3)) == np.array([[[5.5, -1.5]]])
    assert arr3.max((0, 4)) == np.array([[[1.5, 3.5, 5.5]]])


def test_array_mean():
    arr = np.array([[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]])
    assert arr.mean() == 4.5
    assert arr.mean(0) == np.array([[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]])
    assert arr.mean(1) == np.array([[3.5, 4.5, 5.5]])
    assert arr.mean(2) == np.array([[2.5, 4.5, 6.5]])


def test_array_std():
    arr = np.array([[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]])
    assert arr.std() == 1.8257418583505538
    assert arr.std(0) == np.array([[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]])
    assert np.allclose(arr.std(1), np.array([[1.632993,  1.632993,  1.632993]])) == 1
    assert np.allclose(arr.std(2), np.array([[0.81649658, 0.81649658, 0.81649658]])) == 1


def test_array_var():
    arr = np.array([[[1.5, 2.5, 3.5], [3.5, 4.5, 5.5], [5.5, 6.5, 7.5]]])
    assert arr.var() == 3.3333333333333335
    assert arr.var(0) == np.array([[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]])
    assert np.allclose(arr.var(1), np.array([[2.66666667, 2.66666667, 2.66666667]])) == 1
    assert np.allclose(arr.var(2), np.array([[0.66666667, 0.66666667, 0.66666667]])) == 1


if __name__ == "__main__":
    pytest.main()
