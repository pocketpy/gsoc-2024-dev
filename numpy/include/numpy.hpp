#pragma once

#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xsort.hpp> 
#include <xtensor/xview.hpp> 
#include <xtensor-blas/xlinalg.hpp>
#include <cstdint>
#include <complex>
#include <type_traits>
#include <variant>

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
using _Number = std::variant<int_, float_, bool_>;

template <typename T>
class ndarray {
public:
    // Constructor for xtensor xarray
    ndarray() = default;
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
    template <typename U>
    auto operator+(const U& other) const {
        return binary_operator_add_impl<U>(other);
    }
    template <typename U>
    auto binary_operator_add_impl(const U& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) + other;
        return ndarray<result_type>(result);
    }
    template <>
    auto binary_operator_add_impl(const float_& other) const {
        xt::xarray<float_> result = xt::cast<float_>(_array) + other;
        return ndarray<float_>(result);
    }

    template <typename U>
    auto operator-(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) - xt::cast<result_type>(other.get_array());
        return ndarray<result_type>(result);
    }
    template <typename U>
    auto operator-(const U& other) const {
        return binary_operator_sub_impl<U>(other);
    }
    template <typename U>
    auto binary_operator_sub_impl(const U& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) - other;
        return ndarray<result_type>(result);
    }
    template <>
    auto binary_operator_sub_impl(const float_& other) const {
        xt::xarray<float_> result = xt::cast<float_>(_array) - other;
        return ndarray<float_>(result);
    }

    template <typename U>
    auto operator*(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) * xt::cast<result_type>(other.get_array());
        return ndarray<result_type>(result);
    }
    template <typename U>
    auto operator*(const U& other) const {
        return binary_operator_mul_impl<U>(other);
    }
    template <typename U>
    auto binary_operator_mul_impl(const U& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) * other;
        return ndarray<result_type>(result);
    }
    template <>
    auto binary_operator_mul_impl(const float_& other) const {
        xt::xarray<float_> result = xt::cast<float_>(_array) * other;
        return ndarray<float_>(result);
    }

    template <typename U>
    auto operator/(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) / xt::cast<result_type>(other.get_array());
        return ndarray<result_type>(result);
    }
    template <typename U>
    auto operator/(const U& other) const {
        return binary_operator_truediv_impl<U>(other);
    }
    template <typename U>
    auto binary_operator_truediv_impl(const U& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::cast<result_type>(_array) / other;
        return ndarray<result_type>(result);
    }
    template <>
    auto binary_operator_truediv_impl(const float_& other) const {
        xt::xarray<float_> result = xt::cast<float_>(_array) / other;
        return ndarray<float_>(result);
    }

    template <typename U>
    auto pow(const ndarray<U>& other) const {
        using result_type = std::common_type_t<T, U>;
        xt::xarray<result_type> result = xt::pow(xt::cast<result_type>(_array), xt::cast<result_type>(other.get_array()));
        return ndarray<result_type>(result);
    }
    template <typename U>
    auto pow(const U& other) const {
        return pow_impl<U>(other);
    }
    template <typename U>
    auto pow_impl(const U& other) const {
        xt::xarray<float_> result = xt::pow(xt::cast<float_>(_array), other);
        return ndarray<float_>(result);
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
    template <typename U>
    ndarray operator&(const U& other) const {
        xt::xarray<int_> result = _array & other;
        return ndarray(result);
    }

    template <typename U>
    ndarray operator|(const ndarray<U>& other) const {
        xt::xarray<int_> result = _array | other.get_array();
        return ndarray(result);
    }
    template <typename U>
    ndarray operator|(const U& other) const {
        xt::xarray<int_> result = _array | other;
        return ndarray(result);
    }

    template <typename U>
    ndarray operator^(const ndarray<U>& other) const {
        xt::xarray<int_> result = _array ^ other.get_array();
        return ndarray(result);
    }
    template <typename U>
    ndarray operator^(const U& other) const {
        xt::xarray<int_> result = _array ^ other;
        return ndarray(result);
    }

    ndarray operator~() const { return ndarray(~(_array)); }

    T operator[](int index) const { return _array[index]; }
    template <typename... Args>
    T operator()(Args... args) const {
        return _array(args...);
    }

    T& operator[](int index) { return _array[index]; }
    template <typename... Args>
    T& operator()(Args... args) {
        return _array(args...);
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
    template <typename... Args>
    ndarray<T> sum(Args... args) const {
        xt::xarray<T> result = xt::sum(_array, {args...});
        return ndarray<T>(result);
    }

    ndarray<T> prod() const {
        return ndarray<T>(xt::prod(_array));
    }
    ndarray<T> prod(int axis) const {
        xt::xarray<T> result = xt::prod(_array, {axis});
        return ndarray<T>(result);
    }
    template <typename... Args>
    ndarray<T> prod(Args... args) const {
        xt::xarray<T> result = xt::prod(_array, {args...});
        return ndarray<T>(result);
    }

    ndarray<T> min() const {
        return ndarray<T>(xt::amin(_array));
    }
    ndarray<T> min(int axis) const {
        xt::xarray<T> result = xt::amin(_array, {axis});
        return ndarray<T>(result);
    }
    template <typename... Args>
    ndarray<T> min(Args... args) const {
        xt::xarray<T> result = xt::amin(_array, {args...});
        return ndarray<T>(result);
    }

    ndarray<T> max() const {
        return ndarray<T>(xt::amax(_array));
    }
    ndarray<T> max(int axis) const {
        xt::xarray<T> result = xt::amax(_array, {axis});
        return ndarray<T>(result);
    }
    template <typename... Args>
    ndarray<T> max(Args... args) const {
        xt::xarray<T> result = xt::amax(_array, {args...});
        return ndarray<T>(result);
    }

    ndarray<T> mean() const {
        return ndarray<T>(xt::mean(_array));
    }
    ndarray<T> mean(int axis) const {
        xt::xarray<T> result = xt::mean(_array, {axis});
        return ndarray<T>(result);
    }
    template <typename... Args>
    ndarray<T> mean(Args... args) const {
        xt::xarray<T> result = xt::mean(_array, {args...});
        return ndarray<T>(result);
    }

    ndarray<T> std() const {
        return ndarray<T>(xt::stddev(_array));
    }
    ndarray<T> std(int axis) const {
        xt::xarray<T> result = xt::stddev(_array, {axis});
        return ndarray<T>(result);
    }
    template <typename... Args>
    ndarray<T> std(Args... args) const {
        xt::xarray<T> result = xt::stddev(_array, {args...});
        return ndarray<T>(result);
    }

    ndarray<T> var() const {
        return ndarray<T>(xt::variance(_array));
    }
    ndarray<T> var(int axis) const {
        xt::xarray<T> result = xt::variance(_array, {axis});
        return ndarray<T>(result);
    }
    template <typename... Args>
    ndarray<T> var(Args... args) const {
        xt::xarray<T> result = xt::variance(_array, {args...});
        return ndarray<T>(result);
    }

    private:
        xt::xarray<T> _array;

};

template <typename T>
std::ostream& operator<<(std::ostream& os, const ndarray<T>& arr) {
    os << arr.get_array();
    return os;
}

} // namespace pkpy