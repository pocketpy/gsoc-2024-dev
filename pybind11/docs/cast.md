# Type Conversion

How does pybind11 achieve bidirectional function calls between C++ and Python? The key is implicit cast. pybind11 performs appropriate type conversions during function calls to ensure correct invocation.

# C++ -> Python

| C++                                               | To Python | From Python         |
| ------------------------------------------------- | --------- | ------------------- |
| bool                                              | bool      | bool                |
| integer                                           | int       | int                 |
| float, double                                     | float     | float, int(convert) |
| char, const char\*, std::string, std::string_view | str       | str, bytes(convert) |
| std::vector\<T\>                                  | list      | list                |
| std::array\<T, N\>                                | list      | list                |
| std::tuple\<T...\>                                | tuple     | tuple               |
| std::pair\<T1, T2\>                               | tuple     | tuple               |
| std::map\<K, V\>                                  | dict      | dict                |
| std::unordered_map\<K, V\>                        | dict      | dict                |
| std::set\<T\>                                     | set       | set                 |
| std::unordered_set\<T\>                           | set       | set                 |
| Python class                                      | wrapper   | wrapper             |
| T                                                 | instance  | instance            |
| T\*                                               | instance  | instance            |
| T&, T&&                                           | instance  | instance            |

# Custom Type Conversion

In pybind11, type conversion is handled by `type_caster`. A qualified `type_caster` has the following structure:

```cpp
template<typename T>
struct type_caster {
    T value;

    static bool load(const handle &h, bool convert);

    template<typename U>
    static handle cast(U&& src, return_value_policy policy, handle parent);
};
```

The `load` function is responsible for converting a Python object to a C++ object. `h` is the handle to the Python object, and `convert` indicates whether type conversion is needed. The `cast` function is responsible for converting a C++ object to a Python object. `src` is a reference to the C++ object, `policy` is the return value policy, and `parent` is the handle to the parent object.
