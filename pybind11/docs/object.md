# Types

## handle

```cpp
class handle{
    // ...
public:
    handle() = default;
    handle(const handle& other) = default;
    handle& operator=(const handle& other) = default;
}
```

对 Python 对象的轻量级包装，不持有 Python 对象的引用。常用于中间计算结果的传递。

成员函数

#### cast

```cpp
template <typename T>
T cast() const;
```

把 Python 对象转换为 C++ 对象。如果转换失败，会抛出异常。

#### is/is_none

```cpp
bool is(const handle& other) const;
bool is_none() const;
```

判断两个`handle`是否指向同一个 Python 对象，`is_none` 判断是否为`None`对象。`obj,is(other)` 等同于在 Python 中执行 `obj is other`。`is_none` 等同于在 Python 中执行 `obj is None`。

#### in/contains

```cpp
bool in(const handle& other) const;
bool contains(const handle& other) const;
```

判断对象是否在另一个对象中。例如，`obj.in(container)` 等同于在 Python 中执行 `obj in container`。`contains` 则是反向操作。

#### begin/end

```cpp
iterator begin() const;
iterator end() const;
```

用于迭代对象，具体细节参考[迭代器](#iterator)。

#### doc

FIXME: 支持对 doc 赋值

```cpp
str doc() const;
```

获取对象的字符串表示和文档字符串。`str` 等同于在 Python 中执行 `obj.__str__`，`doc` 等同于在 Python 中执行 `obj.__doc__`。

#### attr

```cpp
attr_accessor attr(const char* name) const;
attr_accessor attr(const handle& name) const;
attr_accessor attr(object&& name) const;
```

用于访问对象的属性，`obj.attr(name)` 效果等同于在 Python 中执行 `obj.name`。

注意：`attr`方法并不直接返回`object`，而是一个`attr_accessor`。为什么这样设计呢？

考虑如下的 Python 代码

```python
obj = object()
obj.x = 1 # modify successfully

x = obj.x
x = 3 # modify failed
```

尝试在 C++ 中进行等价表达

```cpp
py::object obj = py::object();
obj.attr("x") = 1;

py::object x = obj.attr("x");
x = 3;
```

为了修改`obj`的`x`属性，我们实际上需要调用`setattr`方法，而这个方法需要父对象作为参数。如果直接返回 object，里面并不会存储父对象，因此无法实现赋值效果。所以我们转而返回`attr_accessor`，它会存储父对象的引用。下一个问题就是如何区分上面两种情况，一种调用`setattr`，而另一种不调用呢？这可以通过对成员函数引用限定来实现，事实上第一种情况，是`this`对象是右值，而第二种情况，`this`对象是左值。

#### operator[]

```cpp
item_accessor operator[](int64_t key) const;
item_accessor operator[](const char* key) const;
item_accessor operator[](const handle& key) const;
item_accessor operator[](object&& key) const;
```

`obj[key]` 等同于在 Python 中执行`obj[key]`，类似于`attr`，在`inplace`的时候调用`__setitem__`方法。

#### operator()

```cpp
template <typename... Args>
object operator()(Args&&... args) const;
```

将每个 C++ 参数转换为 Python 对象，然后调用 Python 对象的`__call__`方法。

#### other operators

```cpp
object operator- () const;
object operator~() const;

friend object operator+ (const handle& lhs, const handle& rhs);
friend object operator- (const handle& lhs, const handle& rhs);
friend object operator* (const handle& lhs, const handle& rhs);
friend object operator% (const handle& lhs, const handle& rhs);
friend object operator/ (const handle& lhs, const handle& rhs);
friend object operator| (const handle& lhs, const handle& rhs);
friend object operator& (const handle& lhs, const handle& rhs);
friend object operator^ (const handle& lhs, const handle& rhs);
friend object operator<< (const handle& lhs, const handle& rhs);
friend object operator>> (const handle& lhs, const handle& rhs);

friend object operator+= (handle& lhs, const handle& rhs);
friend object operator-= (handle& lhs, const handle& rhs);
friend object operator*= (handle& lhs, const handle& rhs);
friend object operator/= (handle& lhs, const handle& rhs);
friend object operator%= (handle& lhs, const handle& rhs);
friend object operator|= (handle& lhs, const handle& rhs);
friend object operator&= (handle& lhs, const handle& rhs);
friend object operator^= (handle& lhs, const handle& rhs);
friend object operator<<= (handle& lhs, const handle& rhs);
friend object operator>>= (handle& lhs, const handle& rhs);

friend object operator== (const handle& lhs, const handle& rhs);
friend object operator!= (const handle& lhs, const handle& rhs);
friend object operator< (const handle& lhs, const handle& rhs);
friend object operator> (const handle& lhs, const handle& rhs);
friend object operator<= (const handle& lhs, const handle& rhs);
friend object operator>= (const handle& lhs, const handle& rhs);
```

效果上等同于在 Python 中使用对应的运算符。

## object

```cpp
class object : public handle {
    // ...
public:
    object();
    object(const object& other) = delete;
    object& operator=(const object& other) = delete;
    object(object&& other) noexcept;
    object& operator=(object&& other) noexcept;
    ~object();
};
```

继承自 handle, 持有 Python 对象的引用。

> 如果需要在栈上或者容器中存储 Python 对象，需要使用 object 类型。

#### Why distinguish handle and object?

考虑如下的代码示例：

```cpp
py::module_ m = py::module_::import("math");
py::handle h = m.attr("cos")(3.141592653);
py::exec("print('Hello, World!')");
// 可能执行 GC, 从而导致 h 失效
float f = h.cast<float>(); // maybe use after free
```

使用 object 则可以避免这种情况发生

```cpp
py::module_ m = py::module_::import("math");
py::object o = m.attr("cos")(3.141592653);
py::exec("print('Hello, World!')");
// o 持有 Python 对象的引用, 不会被 GC 回收
float f = o.cast<float>(); // safe
```

object 类型在构造的时候会持有 Python 对象的引用，析构的时候会释放引用，不会导致内存泄露。

> pocketpy 使用纯 GC 的方式进行垃圾回收（CPython 是引用计数 + GC），那我们是如何实现这个效果的呢？原来 pocketpy 提供了一个函数指针成员：\_gc_marker_ex，该函数会在每次 GC 之前调用。于是我们只需要建立一个全局的 map，利用它实现类似引用计数的效果即可。

## type

```cpp
class type : public object {
    // ...
};
```

Python 中 `type` 对象的包装。

#### handle_of

```cpp
static handle handle_of(const handle& obj);
```

返回对象的类型

```cpp
template <typename T>
static handle handle_of();
```

返回`T`类型在 Python 中注册的类型，如果没有注册，则返回`None`。

#### of

```cpp
static type of(const handle& obj);

template <typename T>
static type of();
```

同`handle_of`，但返回的是`type`对象，持有引用。

## iterable

```cpp
class iterable : public object {
    // ...
};
```

Python 中可迭代对象的包装。如果一个对象实现了`__iter__`方法，那么它就是可迭代的。

## iterator

```cpp
class iterator : public object {
    // ...
};
```

Python 中迭代器的包装。如果一个对象实现了`__iter__`和`__next__`方法，那么它就是迭代器。

```cpp
static iterator sentinel() { return {}; }
```

返回一个空的迭代器，用于标记迭代结束。

## list

```cpp
class list : public object {
    // ...
};
```

Python 中列表的包装。

## tuple

```cpp
class tuple : public object {
    // ...
};
```

## set

```cpp
class set : public object {
    // ...
};
```

## dict

```cpp
class dict : public object {
    // ...
};
```

## str

```cpp
class str : public object {
    // ...
};
```

## bytes

```cpp
class bytes : public object {
    // ...
};
```

## int\_

```cpp
class int_ : public object {
    // ...
};
```

## float\_

```cpp
class float_ : public object {
    // ...
};
```

## bool\_

```cpp
class bool_ : public object {
    // ...
};
```

## none

```cpp
class none : public object {
    // ...
};
```
