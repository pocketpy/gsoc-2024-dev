#pragma once

#include "error.h"

namespace pkbind {

#define PKBIND_TYPE_IMPL(parent, expr)                                                                                 \
                                                                                                                       \
private:                                                                                                               \
    friend class type;                                                                                                 \
    static auto type_or_check() { return expr; }                                                                       \
                                                                                                                       \
public:                                                                                                                \
    using parent::parent;                                                                                              \
    using parent::operator=;

class type : public object {
protected:
    inline static std::unordered_map<std::type_index, type> m_type_map;

    template <typename T>
    constexpr inline static bool is_check_v = std::is_invocable_r_v<bool, decltype(T::type_or_check()), handle>;

public:
    PKBIND_TYPE_IMPL(object, tp_type);

    // note: type is global instance, so we use ref_t.
    type(py_Type type) : object(py_tpobject(type), ref_t{}) {}

    py_Type index() const { return py_totype(ptr()); }

    const char* name() const { return py_tpname(index()); }

    static type of(handle h) { return py_typeof(h.ptr()); }

    template <typename T>
    static type of();

    template <typename T>
    static bool isinstance(handle obj);
};

class none : public object {
    PKBIND_TYPE_IMPL(object, tp_NoneType);

    // note: none is global instance, so we use ref_t.
    none() : object(py_None, ref_t{}) {}
};

class bool_ : public object {
    PKBIND_TYPE_IMPL(object, tp_bool);

    // same as none, bool is a singleton.
    bool_(bool value) : object(value ? py_True : py_False, ref_t{}) {}

    explicit operator bool () { return py_tobool(ptr()); }
};

class int_ : public object {
    PKBIND_TYPE_IMPL(object, tp_int);

    int_(py_i64 value) : object(alloc_t{}) { py_newint(m_ptr, value); }

    explicit operator py_i64 () { return py_toint(ptr()); }
};

class float_ : public object {
    PKBIND_TYPE_IMPL(object, tp_float);

    float_(py_f64 value) : object(alloc_t{}) { py_newfloat(m_ptr, value); }

    explicit operator py_f64 () { return py_tofloat(ptr()); }
};

bool hasattr(handle obj, name name);

class iterable : public object {
    PKBIND_TYPE_IMPL(object, [](handle h) {
        return hasattr(h, "__iter__");
    });
};

class iterator : public object {
    template <typename Dervied>
    friend class interface;

    iterator() : object(), m_value() {}

    iterator(handle h) : object(h, realloc_t{}), m_value() { operator++ (); }

public:
    PKBIND_TYPE_IMPL(object, [](handle h) {
        return hasattr(h, "__iter__") && hasattr(h, "__next__");
    });

    iterator& operator++ () {
        int result = raise_call<py_next>(m_ptr);
        if(result == 1) {
            m_value = object(retv, realloc_t{});
        } else if(result == 0) {
            m_value = object();
        }
        return *this;
    }

    handle operator* () const { return m_value; }

    friend bool operator== (const iterator& lhs, const iterator& rhs) { return lhs.m_value.ptr() == rhs.m_value.ptr(); }

    friend bool operator!= (const iterator& lhs, const iterator& rhs) { return !(lhs == rhs); }

    static iterator sentinel() { return iterator(); }

private:
    object m_value;
};

template <typename Dervied>
iterator interface<Dervied>::begin() const {
    raise_call<py_iter>(ptr());
    return iterator(retv);
}

template <typename Dervied>
iterator interface<Dervied>::end() const {
    return iterator::sentinel();
}

class str : public object {
    PKBIND_TYPE_IMPL(object, tp_str);

    str(const char* data, int size) : object(alloc_t{}) { py_newstrn(m_ptr, data, size); }

    str(const char* data) : str(data, strlen(data)) {}

    str(std::string_view s) : str(s.data(), static_cast<int>(s.size())) {}
};

class tuple : public object {
    PKBIND_TYPE_IMPL(object, tp_tuple);

    tuple(int size) : object(alloc_t{}) { py_newtuple(m_ptr, size); }

    template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 1)>>
    tuple(Args&&... args);

    tuple_accessor operator[] (int index) const;

    int size() const { return py_tuple_len(m_ptr); }

    bool empty() const { return size() == 0; }

    class iterator {
        friend tuple;

        iterator(py_Ref ptr, int index) : ptr(ptr), index(index) {}

    public:
        bool operator== (const iterator& other) const { return index == other.index; }

        bool operator!= (const iterator& other) const { return index != other.index; }

        iterator& operator++ () {
            index++;
            return *this;
        }

        handle operator* () const { return py_tuple_getitem(ptr, index); }

    private:
        py_Ref ptr;
        int index;
    };

    iterator begin() const { return iterator(m_ptr, 0); }

    iterator end() const { return iterator(m_ptr, size()); }
};

class list : public object {
    PKBIND_TYPE_IMPL(object, tp_list);

    list() : object(alloc_t{}) { py_newlist(m_ptr); }

    list(int size) : object(alloc_t{}) { py_newlistn(m_ptr, size); }

    template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 1)>>
    list(Args&&... args);

    list_accessor operator[] (int index) const;

    int size() const { return py_list_len(m_ptr); }

    bool empty() const { return size() == 0; }

    void swap(int i, int j) { py_list_swap(m_ptr, i, j); }

    void append(handle item) { py_list_append(m_ptr, item.ptr()); }

    void insert(int index, handle item) { py_list_insert(m_ptr, index, item.ptr()); }

    void remove(int index) { py_list_delitem(m_ptr, index); }

    void clear() { py_list_clear(m_ptr); }

    class iterator {
        friend list;

        iterator(py_Ref ptr, int index) : ptr(ptr), index(index) {}

    public:
        iterator(const iterator&) = default;

        bool operator== (const iterator& other) const { return index == other.index; }

        bool operator!= (const iterator& other) const { return index != other.index; }

        iterator operator++ () {
            index++;
            return *this;
        }

        handle operator* () const { return py_list_getitem(ptr, index); }

    private:
        py_Ref ptr;
        int index;
    };

    iterator begin() const { return iterator(m_ptr, 0); }

    iterator end() const { return iterator(m_ptr, size()); }
};

class dict : public object {
    PKBIND_TYPE_IMPL(object, tp_dict);

    dict() : object(alloc_t{}) { py_newdict(m_ptr); }

    template <typename... Args,
              typename = std::enable_if_t<(std::is_same_v<remove_cvref_t<Args>, class arg_with_default> && ...)>>
    dict(Args&&... args);

    dict_accessor operator[] (int key) const;

    dict_accessor operator[] (name key) const;

    dict_accessor operator[] (handle key) const;

    int size() const { return py_dict_len(m_ptr); }

    bool empty() const { return size() == 0; }

    template <typename Fn>
    void apply(Fn&& fn) {
        static_assert(std::is_invocable_v<Fn, py_Ref, py_Ref>);
        using ret = std::invoke_result_t<Fn, py_Ref, py_Ref>;
        auto callback = +[](py_Ref key, py_Ref value, void* data) -> bool {
            auto fn = reinterpret_cast<Fn*>(data);
            if constexpr(std::is_same_v<ret, void>) {
                (*fn)(key, value);
                return true;
            } else {
                return (*fn)(key, value);
            }
        };
        raise_call<py_dict_apply>(m_ptr, callback, &fn);
    }

    class iterator {
        friend dict;

        iterator(py_Ref ptr) : ptr(ptr) {
            raise_call<py_iter>(ptr);
            iter = py_retval();
        }

    public:
        iterator(const iterator&) = default;

        bool operator== (const iterator& other) const { return iter == other.iter; }

        bool operator!= (const iterator& other) const { return !(*this == other); }

        iterator& operator++ () {
            int result = raise_call<py_next>(iter);
            if(result == 1) {
                py_setreg(2, py_retval());
                raise_call<py_dict_getitem>(ptr, py_getreg(2));
                py_setreg(3, py_retval());
                temp = {py_getreg(2), py_getreg(3)};
            } else if(result == 0) {
                iter = nullptr;
            }
            return *this;
        }

        std::pair<handle, handle> operator* () const { return temp; }

    private:
        py_Ref ptr;
        py_Ref iter;
        std::pair<handle, handle> temp;
    };

    iterator begin() const { return iterator(m_ptr); }

    iterator end() const { return iterator(nullptr); }
};

class slice : public object {
    PKBIND_TYPE_IMPL(object, tp_slice);
};

class set : public object {};

class args : public tuple {
    PKBIND_TYPE_IMPL(tuple, tp_tuple);
};

class kwargs : public dict {
    PKBIND_TYPE_IMPL(dict, tp_dict);
};

// TODO:
class capsule : public object {};

}  // namespace pkbind
