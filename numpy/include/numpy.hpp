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

    private:
        xt::xarray<T> _array;

};

template <typename T>
std::ostream& operator<<(std::ostream& os, const ndarray<T>& arr) {
    os << arr.get_array();
    return os;
}

} // namespace pkpy