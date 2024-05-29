# 类型转换

pybind11 如何做到 C++ 和 Python 中函数函数的双向调用的呢？关键就是 implicit cast。pybind11 在函数调用的时候会适当的进行类型转换从而保证调用正确。

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

# 自定义类型转换

在 pybind11 中，类型转换是由`type_caster`负责的，一个合格的`type_caster`具有以下结构

```cpp
template<typename T>
struct type_caster {
    T value;

    static bool load(const handle &h, bool convert);

    template<typename U>
    static handle cast(U&& src, return_value_policy policy, handle parent);
};
```

其中`load`函数负责 Python 对象到 C++ 对象的转换，`h`是 Python 对象的句柄，`convert`表示是否需要进行类型转换。`cast`函数负责 C++ 对象到 Python 对象的转换，`src`是 C++ 对象的引用，`policy`是返回值的策略，`parent`是父对象的句柄。
