#pragma once

#include <any>
#include <cstdint>
#include <complex>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xsort.hpp> 
#include <xtensor/xview.hpp> 
#include <xtensor-blas/xlinalg.hpp>

namespace pkpy {

// Type aliases
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using int_ = int32;
using float32 = float;
using float64 = double;
using float_ = float64;
using bool_ = bool;
using complex64 = std::complex<float32>;
using complex128 = std::complex<float64>;
using complex_ = complex128;
using string = std::string;

template <typename T>
struct dtype_traits {
    static constexpr const char* name = "unknown";
};

#define REGISTER_DTYPE(Type, Name) \
template <> \
struct dtype_traits<Type> { \
    static constexpr const char* name = Name; \
};

REGISTER_DTYPE(int8_t, "int8");
REGISTER_DTYPE(int16_t, "int16");
REGISTER_DTYPE(int32_t, "int32");
REGISTER_DTYPE(int64_t, "int64");
REGISTER_DTYPE(uint8_t, "uint8");
REGISTER_DTYPE(uint16_t, "uint16");
REGISTER_DTYPE(uint32_t, "uint32");
REGISTER_DTYPE(uint64_t, "uint64");
REGISTER_DTYPE(float_, "float64");
REGISTER_DTYPE(bool_, "bool");
REGISTER_DTYPE(std::complex<float32>, "complex64");
REGISTER_DTYPE(std::complex<float64>, "complex128");

using _Dtype = std::string;
using _ShapeLike = std::vector<int_>;

namespace numpy {

template <typename T>
class ndarray;

template<typename T>
constexpr inline auto is_ndarray_v = false;

template<typename T>
constexpr inline auto is_ndarray_v<ndarray<T>> = true;


template <typename T>
class ndarray {
public:
    // Constructor for xtensor xarray
    ndarray() = default;
    ndarray(const T scalar) : _array(scalar) {}
    ndarray(const xt::xarray<T>& arr) : _array(arr) {}

    // Constructor for mutli-dimensional array
    ndarray(std::initializer_list<T> init_list) : _array(init_list) {}
    ndarray(std::initializer_list<std::initializer_list<T>> init_list) : _array(init_list) {}
    ndarray(std::initializer_list<std::initializer_list<std::initializer_list<T>>> init_list) : _array(init_list) {}
    ndarray(std::initializer_list<std::initializer_list<std::initializer_list<std::initializer_list<T>>>> init_list) : _array(init_list) {}
    ndarray(std::initializer_list<std::initializer_list<std::initializer_list<std::initializer_list<std::initializer_list<T>>>>> init_list) : _array(init_list) {}

    // Accessor function for _array
    const xt::xarray<T>& get_array() const { return _array; }

    // Properties
    _Dtype dtype() const { return dtype_traits<T>::name; }
    int ndim() const { return _array.dimension(); }
    int size() const { return _array.size(); }
    _ShapeLike shape() const { return _ShapeLike(_array.shape().begin(), _array.shape().end()); }

    // Dunder Methods
    template <typename U>
    auto operator+(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) + xt::cast<result_type>(other.get_array());
        return ndarray<result_type>(result);
    }
    template <typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
    auto operator+(const U& other) const {
        return binary_operator_add_impl<U>(other);
    }
    template <typename U>
    auto binary_operator_add_impl(const U& other) const {
        if constexpr (std::is_same_v<U, float_>) {
            xt::xarray<float_> result = xt::cast<float_>(_array) + other;
            return ndarray<float_>(result);
        } else {
            using result_type = std::common_type_t<T, U>;
            xt::xarray<result_type> result = xt::cast<result_type>(_array) + other;
            return ndarray<result_type>(result);   
        }
    }

    template <typename U>
    auto operator-(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) - xt::cast<result_type>(other.get_array());
        return ndarray<result_type>(result);
    }
    template <typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
    auto operator-(const U& other) const {
        return binary_operator_sub_impl<U>(other);
    }
    template <typename U>
    auto binary_operator_sub_impl(const U& other) const {
        if constexpr (std::is_same_v<U, float_>) {
            xt::xarray<float_> result = xt::cast<float_>(_array) - other;
            return ndarray<float_>(result);
        } else {
            using result_type = std::common_type_t<T, U>;
            xt::xarray<result_type> result = xt::cast<result_type>(_array) - other;
            return ndarray<result_type>(result);
        }
    }

    template <typename U>
    auto operator*(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) * xt::cast<result_type>(other.get_array());
        return ndarray<result_type>(result);
    }
    template <typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
    auto operator*(const U& other) const {
        return binary_operator_mul_impl<U>(other);
    }
    template <typename U>
    auto binary_operator_mul_impl(const U& other) const {
        xt::xarray<float_> result = xt::cast<float_>(_array) * other;
        return ndarray<float_>(result);
    }

    template <typename U>
    auto operator/(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) / xt::cast<result_type>(other.get_array());
        return ndarray<result_type>(result);
    }
    template <typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
    auto operator/(const U& other) const {
        return binary_operator_truediv_impl<U>(other);
    }
    template <typename U>
    auto binary_operator_truediv_impl(const U& other) const {
        xt::xarray<float_> result = xt::cast<float_>(_array) / other;
        return ndarray<float_>(result);
    }

    template <typename U>
    auto pow(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::pow(xt::cast<result_type>(_array), xt::cast<result_type>(other.get_array()));
        return ndarray<result_type>(result);
    }
    template <typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
    auto pow(const U& other) const {
        return pow_impl<U>(other);
    }
    template <typename U>
    auto pow_impl(const U& other) const {
        if constexpr (std::is_same_v<U, float_>) {
            xt::xarray<float_> result = xt::pow(xt::cast<float_>(_array), other);
            return ndarray<float_>(result);
        } else {
            using result_type = std::common_type_t<T, U>;
            xt::xarray<result_type> result = xt::pow(xt::cast<result_type>(_array), other);
            return ndarray<result_type>(result);
        }
    }

    template <typename U>
    auto matmul(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::linalg::dot(xt::cast<result_type>(_array), xt::cast<result_type>(other.get_array()));
        return ndarray<result_type>(result);
    }

    template <typename U>
    ndarray operator&(const ndarray<U>& other) const {
        xt::xarray<int_> result = _array & other.get_array();
        return ndarray(result);
    }
    template <typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
    ndarray operator&(const U& other) const {
        xt::xarray<int_> result = _array & other;
        return ndarray(result);
    }

    template <typename U>
    ndarray operator|(const ndarray<U>& other) const {
        xt::xarray<int_> result = _array | other.get_array();
        return ndarray(result);
    }
    template <typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
    ndarray operator|(const U& other) const {
        xt::xarray<int_> result = _array | other;
        return ndarray(result);
    }

    template <typename U>
    ndarray operator^(const ndarray<U>& other) const {
        xt::xarray<int_> result = _array ^ other.get_array();
        return ndarray(result);
    }
    template <typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
    ndarray operator^(const U& other) const {
        xt::xarray<int_> result = _array ^ other;
        return ndarray(result);
    }

    ndarray operator~() const { return ndarray(~(_array)); }
    
    ndarray operator[](int index) const { return ndarray(xt::view(_array, index, xt::all())); }
    ndarray operator[](const std::vector<int>& indices) const { return ndarray(xt::view(_array, xt::keep(indices))); }
    ndarray operator[](const std::tuple<int, int, int>& slice) const {
        return ndarray(xt::view(_array, xt::range(std::get<0>(slice), std::get<1>(slice), std::get<2>(slice))));
    }
    template <typename... Args>
    T operator()(Args... args) const {
        return _array(args...);
    }

    void set_item(int index, const ndarray<T>& value) {
        xt::view(_array, index, xt::all()) = value.get_array();
    }
    void set_item(const std::vector<int>& indices, const ndarray<T>& value) {
        xt::view(_array, xt::keep(indices)) = value.get_array();
    }
    void set_item(const std::tuple<int, int, int>& slice, const ndarray<T>& value) {
        xt::view(_array, xt::range(std::get<0>(slice), std::get<1>(slice), std::get<2>(slice))) = value.get_array();
    }
    void set_item_2d(int i1, int i2, T value) {
        _array(i1, i2) = value;
    }
    void set_item_3d(int i1, int i2, int i3, T value) {
        _array(i1, i2, i3) = value;
    }
    void set_item_4d(int i1, int i2, int i3, int i4, T value) {
        _array(i1, i2, i3, i4) = value;
    }
    void set_item_5d(int i1, int i2, int i3, int i4, int i5, T value) {
        _array(i1, i2, i3, i4, i5) = value;
    }

    // Boolean Functions
    bool all() const { return xt::all(_array); }
    bool any() const { return xt::any(_array); }

    // Aggregate Functions
    ndarray<T> sum() const {
        return ndarray<T>(xt::sum(_array));
    }
    ndarray<T> sum(int axis) const {
        xt::xarray<T> result = xt::sum(_array, {axis});
        return ndarray<T>(result);
    }
    ndarray<T> sum(const _ShapeLike& axis) const {
        xt::xarray<T> result = xt::sum(_array, axis);
        return ndarray<T>(result);
    }

    ndarray<T> prod() const {
        return ndarray<T>(xt::prod(_array));
    }
    ndarray<T> prod(int axis) const {
        xt::xarray<T> result = xt::prod(_array, {axis});
        return ndarray<T>(result);
    }
    ndarray<T> prod(const _ShapeLike& axes) const {
        xt::xarray<T> result = xt::prod(_array, axes);
        return ndarray<T>(result);
    }

    ndarray<T> min() const {
        return ndarray<T>(xt::amin(_array));
    }
    ndarray<T> min(int axis) const {
        xt::xarray<T> result = xt::amin(_array, {axis});
        return ndarray<T>(result);
    }
    ndarray<T> min(const _ShapeLike& axes) const {
        xt::xarray<T> result = xt::amin(_array, axes);
        return ndarray<T>(result);
    }

    ndarray<T> max() const {
        return ndarray<T>(xt::amax(_array));
    }
    ndarray<T> max(int axis) const {
        xt::xarray<T> result = xt::amax(_array, {axis});
        return ndarray<T>(result);
    }
    ndarray<T> max(const _ShapeLike& axes) const {
        xt::xarray<T> result = xt::amax(_array, axes);
        return ndarray<T>(result);
    }

    ndarray<T> mean() const {
        return ndarray<T>(xt::mean(_array));
    }
    ndarray<T> mean(int axis) const {
        xt::xarray<T> result = xt::mean(_array, {axis});
        return ndarray<T>(result);
    }
    ndarray<T> mean(const _ShapeLike& axes) const {
        xt::xarray<T> result = xt::mean(_array, axes);
        return ndarray<T>(result);
    }

    ndarray<T> std() const {
        return ndarray<T>(xt::stddev(_array));
    }
    ndarray<T> std(int axis) const {
        xt::xarray<T> result = xt::stddev(_array, {axis});
        return ndarray<T>(result);
    }
    ndarray<T> std(const _ShapeLike& axes) const {
        xt::xarray<T> result = xt::stddev(_array, axes);
        return ndarray<T>(result);
    }

    ndarray<T> var() const {
        return ndarray<T>(xt::variance(_array));
    }
    ndarray<T> var(int axis) const {
        xt::xarray<T> result = xt::variance(_array, {axis});
        return ndarray<T>(result);
    }
    ndarray<T> var(const _ShapeLike& axes) const {
        xt::xarray<T> result = xt::variance(_array, axes);
        return ndarray<T>(result);
    }

    // Searching and Sorting Functions
    ndarray<T> argmin() const {
        return ndarray<T>(xt::argmin(_array));
    }
    ndarray<T> argmin(int axis) const {
        xt::xarray<T> result = xt::argmin(_array, {axis});
        return ndarray<T>(result);
    }

    ndarray<T> argmax() const {
        return ndarray<T>(xt::argmax(_array));
    }
    ndarray<T> argmax(int axis) const {
        xt::xarray<T> result = xt::argmax(_array, {axis});
        return ndarray<T>(result);
    }

    ndarray<T> argsort() const {
        return ndarray<T>(xt::argsort(_array));
    }
    ndarray<T> argsort(int axis) const {
        xt::xarray<T> result = xt::argsort(_array, {axis});
        return ndarray<T>(result);
    }

    ndarray<T> sort() const {
        return ndarray<T>(xt::sort(_array));
    }
    ndarray<T> sort(int axis) const {
        xt::xarray<T> result = xt::sort(_array, {axis});
        return ndarray<T>(result);
    }

    // Shape Manipulation Functions
    ndarray<T> reshape(const _ShapeLike& shape) const {
        xt::xarray<T> dummy = _array; 
        dummy.reshape(shape);
        return ndarray<T>(dummy);
    }

    ndarray<T> squeeze() const {
        return ndarray<T>(xt::squeeze(_array));
    }
    ndarray<T> squeeze(int axis) const {
        xt::xarray<T> result = xt::squeeze(_array, {axis});
        return ndarray<T>(result);
    }

    ndarray<T> transpose() const {
        return ndarray<T>(xt::transpose(_array));
    }
    ndarray<T> transpose(const _ShapeLike& permutation) const {
        return ndarray<T>(xt::transpose(_array, permutation));
    }
    template <typename... Args>
    ndarray<T> transpose(Args... args) const {
        xt::xarray<T> result = xt::transpose(_array, {args...});
        return ndarray<T>(result);
    }

    ndarray<T> repeat(int repeats, int axis) const {
        return ndarray<T>(xt::repeat(_array, repeats, axis));
    }
    ndarray<T> repeat(const std::vector<size_t>& repeats, int axis ) const {
        return ndarray<T>(xt::repeat(_array, repeats, axis));
    }

    ndarray<T> flatten() const {
        return ndarray<T>(xt::flatten(_array));
    }

    // Miscellaneous Functions
    template <typename U>
    ndarray<U> astype() const {
        xt::xarray<U> result = xt::cast<U>(_array);
        return ndarray<U>(result);
    }

    ndarray<T> copy() const {
        ndarray<T> result = *this;
        return result;
    }

    private:
        xt::xarray<T> _array;
};

template <typename T>
ndarray<T> adapt(const std::vector<T>& init_list) {
    return ndarray<T>(xt::adapt(init_list));
}

template <typename T>
ndarray<T> adapt(const std::vector<std::vector<T>>& init_list) {
    std::vector<T> flat_list;
    for(auto row : init_list) {
        for(auto elem : row) {
            flat_list.push_back(elem);
        }
    }
    std::vector<size_t> sh = {init_list.size(), init_list[0].size()};
    return ndarray<T>(xt::adapt(flat_list, sh));
}

template<typename T>
ndarray<T> adapt(const std::vector<std::vector<std::vector<T>>>& init_list) {
    std::vector<T> flat_list;
    for(auto row : init_list) {
        for(auto elem : row) {
            for(auto val : elem) {
                flat_list.push_back(val);
            }
        }
    }
    std::vector<size_t> sh = {init_list.size(), init_list[0].size(), init_list[0][0].size()};
    return ndarray<T>(xt::adapt(flat_list, sh));
}

template<typename T>
ndarray<T> adapt(const std::vector<std::vector<std::vector<std::vector<T>>>>& init_list) {
    std::vector<T> flat_list;
    for(auto row : init_list) {
        for(auto elem : row) {
            for(auto val : elem) {
                for(auto v : val) {
                    flat_list.push_back(v);
                }
            }
        }
    }
    std::vector<size_t> sh = {init_list.size(), init_list[0].size(), init_list[0][0].size(), init_list[0][0][0].size()};
    return ndarray<T>(xt::adapt(flat_list, sh));
}

template<typename T>
ndarray<T> adapt(const std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>& init_list) {
    std::vector<T> flat_list;
    for(auto row : init_list) {
        for(auto elem : row) {
            for(auto val : elem) {
                for(auto v : val) {
                    for(auto v1 : v) {
                        flat_list.push_back(v1);
                    }
                }
            }
        }
    }
    std::vector<size_t> sh = {init_list.size(), init_list[0].size(), init_list[0][0].size(), init_list[0][0][0].size(), init_list[0][0][0][0].size()};
    return ndarray<T>(xt::adapt(flat_list, sh));
}

// Array Creation
template <typename U, typename T>
ndarray<U> array(const std::vector<T>& vec, const _ShapeLike& shape = {}) {
    if (shape.empty()) {
        return ndarray<U>(xt::cast<U>(xt::adapt(vec)));
    } else {
        return ndarray<U>(xt::cast<U>(xt::adapt(vec, shape)));
    }
}

template <typename T>
ndarray<T> zeros(const _ShapeLike& shape) {
    return ndarray<T>(xt::zeros<T>(shape));
}

template <typename T>
ndarray<T> ones(const _ShapeLike& shape) {
    return ndarray<T>(xt::ones<T>(shape));
}

template <typename T>
ndarray<T> full(const _ShapeLike& shape, const T& fill_value) {
    xt::xarray<T> result = xt::ones<T>(shape);
    for(auto it = result.begin(); it != result.end(); ++it) {
        *it = fill_value;
    }
    return ndarray<T>(result);
}

template <typename T>
ndarray<T> identity(int n){
    return ndarray<T>(xt::eye<T>(n));
}

template <typename T>
ndarray<T> arange(const T& stop) {
    return ndarray<T>(xt::arange<T>(stop));
}
template <typename T>
ndarray<T> arange(const T& start, const T& stop) {
    return ndarray<T>(xt::arange<T>(start, stop));
}
template <typename T>
ndarray<T> arange(const T& start, const T& stop, const T& step) {
    return ndarray<T>(xt::arange<T>(start, stop, step));
}

template <typename T>
ndarray<T> linspace(const T& start, const T& stop, int num = 50, bool endpoint = true) {
    return ndarray<T>(xt::linspace<T>(start, stop, num, endpoint));
}

// Trigonometry
template <typename T>
ndarray<float_> sin(const ndarray<T>& arr) {
    return ndarray<float_>(xt::sin(arr.get_array()));
}
ndarray<complex_> sin(const ndarray<complex64>& arr) {
    return ndarray<complex_>(xt::sin(arr.get_array()));
}
ndarray<complex_> sin(const ndarray<complex128>& arr) {
    return ndarray<complex_>(xt::sin(arr.get_array()));
}

template <typename T>
ndarray<float_> cos(const ndarray<T>& arr) {
    return ndarray<float_>(xt::cos(arr.get_array()));
}
ndarray<complex_> cos(const ndarray<complex64>& arr) {
    return ndarray<complex_>(xt::cos(arr.get_array()));
}
ndarray<complex_> cos(const ndarray<complex128>& arr) {
    return ndarray<complex_>(xt::cos(arr.get_array()));
}

template <typename T>
ndarray<float_> tan(const ndarray<T>& arr) {
    return ndarray<float_>(xt::tan(arr.get_array()));
}
ndarray<complex_> tan(const ndarray<complex64>& arr) {
    return ndarray<complex_>(xt::tan(arr.get_array()));
}
ndarray<complex_> tan(const ndarray<complex128>& arr) {
    return ndarray<complex_>(xt::tan(arr.get_array()));
}

template <typename T>
ndarray<float_> arcsin(const ndarray<T>& arr) {
    return ndarray<float_>(xt::asin(arr.get_array()));
}
ndarray<complex_> arcsin(const ndarray<complex64>& arr) {
    return ndarray<complex_>(xt::asin(arr.get_array()));
}
ndarray<complex_> arcsin(const ndarray<complex128>& arr) {
    return ndarray<complex_>(xt::asin(arr.get_array()));
}

template <typename T>
ndarray<float_> arccos(const ndarray<T>& arr) {
    return ndarray<float_>(xt::acos(arr.get_array()));
}
ndarray<complex_> arccos(const ndarray<complex64>& arr) {
    return ndarray<complex_>(xt::acos(arr.get_array()));
}
ndarray<complex_> arccos(const ndarray<complex128>& arr) {
    return ndarray<complex_>(xt::acos(arr.get_array()));
}

template <typename T>
ndarray<float_> arctan(const ndarray<T>& arr) {
    return ndarray<float_>(xt::atan(arr.get_array()));
}
ndarray<complex_> arctan(const ndarray<complex64>& arr) {
    return ndarray<complex_>(xt::atan(arr.get_array()));
}
ndarray<complex_> arctan(const ndarray<complex128>& arr) {
    return ndarray<complex_>(xt::atan(arr.get_array()));
}

// Exponents and Logarithms
template <typename T>
ndarray<float_> exp(const ndarray<T>& arr) {
    return ndarray<float_>(xt::exp(arr.get_array()));
}
ndarray<complex_> exp(const ndarray<complex64>& arr) {
    return ndarray<complex_>(xt::exp(arr.get_array()));
}
ndarray<complex_> exp(const ndarray<complex128>& arr) {
    return ndarray<complex_>(xt::exp(arr.get_array()));
}

template <typename T>
ndarray<float_> log(const ndarray<T>& arr) {
    return ndarray<float_>(xt::log(arr.get_array()));
}
ndarray<complex_> log(const ndarray<complex64>& arr) {
    return ndarray<complex_>(xt::log(arr.get_array()));
}
ndarray<complex_> log(const ndarray<complex128>& arr) {
    return ndarray<complex_>(xt::log(arr.get_array()));
}

template <typename T>
ndarray<float_> log2(const ndarray<T>& arr) {
    return ndarray<float_>(xt::log2(arr.get_array()));
}

template <typename T>
ndarray<float_> log10(const ndarray<T>& arr) {
    return ndarray<float_>(xt::log10(arr.get_array()));
}

// Miscellanous
template <typename T>
ndarray<T> round(const ndarray<T>& arr) {
    return ndarray<T>(xt::round(arr.get_array()));
}

template <typename T>
ndarray<T> floor(const ndarray<T>& arr) {
    return ndarray<T>(xt::floor(arr.get_array()));
}

template <typename T>
ndarray<T> ceil(const ndarray<T>& arr) {
    return ndarray<T>(xt::ceil(arr.get_array()));
}

template <typename T>
auto abs(const ndarray<T>& arr) {
    if constexpr (std::is_same_v<T, complex64> || std::is_same_v<T, complex128>) {
        return ndarray<float_>(xt::abs(arr.get_array()));
    } else {
        return ndarray<T>(xt::abs(arr.get_array()));
    }
}

template <typename T, typename U>
auto concatenate(const ndarray<T>& arr1, const ndarray<U>& arr2, int axis = 0) {
    using result_type = std::common_type_t<T, U>;
    xt::xarray<result_type> xarr1 = xt::cast<result_type>(arr1.get_array());
    xt::xarray<result_type> xarr2 = xt::cast<result_type>(arr2.get_array());
    return ndarray<result_type>(xt::concatenate(xt::xtuple(xarr1, xarr2), axis));
}

// Reverse Dunder Methods
template <typename T, typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
auto operator+(const U& scalar, const ndarray<T>& array) {
    return array + scalar;
}

template <typename T, typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
auto operator-(const U& scalar, const ndarray<T>& array) {
    return (array *(-1)) + scalar;
}

template <typename T, typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
auto operator*(const U& scalar, const ndarray<T>& array) {
    return array * scalar;
}

template <typename T, typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
auto operator/(const U& scalar, const ndarray<T>& array) {
    return array.pow(-1) * scalar;
}

template <typename T, typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
auto operator&(const U& scalar, const ndarray<T>& array) {
    return array & scalar;
}

template <typename T, typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
auto operator|(const U& scalar, const ndarray<T>& array) {
    return array | scalar;
}

template <typename T, typename U, typename = std::enable_if_t<!is_ndarray_v<U>>>
auto operator^(const U& scalar, const ndarray<T>& array) {
    return array ^ scalar;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const ndarray<T>& arr) {
    os << arr.get_array();
    return os;
}

} // namespace numpy
} // namespace pkpy
