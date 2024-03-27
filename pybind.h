#include "pocketpy/include/pocketpy.h"

/**
TODO:
    * support special function
    * support default argument
*/

namespace pkpy::cxx_bind {

template <typename T> bool is_cxx_type(VM *vm, PyObject *obj) {
  if constexpr (std::is_same_v<T, pkpy::Tuple>) {
    return vm->isinstance(obj, VM::tp_tuple);
  } else if constexpr (std::is_same_v<T, pkpy::List>) {
    return vm->isinstance(obj, VM::tp_list);
  } else if constexpr (std::is_same_v<T, pkpy::Dict>) {
    return vm->isinstance(obj, VM::tp_dict);
  } else if constexpr (std::is_same_v<T, pkpy::Str>) {
    return vm->isinstance(obj, VM::tp_str);
  } else if constexpr (std::is_same_v<T, pkpy::Bytes>) {
    return vm->isinstance(obj, VM::tp_bytes);
  } else if constexpr (std::is_same_v<T, pkpy::Function>) {
    return vm->isinstance(obj, VM::tp_function);
  } else if constexpr (std::is_same_v<T, pkpy::DummyModule>) {
    return vm->isinstance(obj, VM::tp_module);
  } else if constexpr (std::is_same_v<T, pkpy::Type>) {
    return vm->isinstance(obj, VM::tp_type);
  } else if constexpr (std::is_same_v<T, bool>) {
    return vm->isinstance(obj, VM::tp_bool);
  } else if constexpr (std::is_integral_v<T>) {
    return vm->isinstance(obj, VM::tp_int);
  } else if constexpr (std::is_floating_point_v<T>) {
    return vm->isinstance(obj, VM::tp_float);
  } else {
    return vm->_cxx_typeid_map.find(typeid(T)) != vm->_cxx_typeid_map.end();
  }
}

template <typename... Args> inline bool check_cxx_types(VM *vm, ArgsView args) {
  if (sizeof...(Args) != args.size()) {
    return false;
  }
  std::size_t index = 0;
  return (is_cxx_type<Args>(vm, args[index++]) && ...);
}

template <typename Ret, typename... Args, std::size_t... Is>
inline PyObject *call_func(VM *vm, Ret (*func)(Args...), ArgsView args,
                           std::index_sequence<Is...>) {
  if (!check_cxx_types<Args...>(vm, args)) {
    return nullptr;
  } else {
    if constexpr (std::is_void_v<Ret>) {
      func(py_cast<Args>(vm, args[Is])...);
      return vm->None;
    } else {
      return py_var(vm, func(py_cast<Args>(vm, args[Is])...));
    }
  }
}

template <typename Ret, typename C, typename... Args, std::size_t... Is>
inline PyObject *call_cxx_method(VM *vm, C *self, Ret (C::*method)(Args...),
                                 ArgsView args, std::index_sequence<Is...>) {
  if (!check_cxx_types<Args...>(vm, args)) {
    return nullptr;
  } else {
    if constexpr (std::is_void_v<Ret>) {
      (self->*method)(py_cast<Args>(vm, args[Is])...);
      return vm->None;
    } else {
      return py_var(vm, (self->*method)(py_cast<Args>(vm, args[Is])...));
    }
  }
}

class module_ {
  template <typename T> friend class class_;

  template <typename T> friend class enum_;

private:
  VM *vm;
  PyObject *mod;

  using CXX_Func = std::function<PyObject *(VM *, ArgsView)>;
  inline static std::map<pkpy::StrName, std::vector<CXX_Func>>
      func_overload_set;

public:
  template <typename Ret, typename... Args, typename... Extra>
  module_ &def(const char *name, Ret (*fn)(Args...), const Extra &...extras) {
    Str name_index = name;
    if (func_overload_set.find(name_index) == func_overload_set.end()) {
      func_overload_set[name_index] = {};
      auto fp = [name_index](VM *vm, ArgsView args) -> PyObject * {
        for (auto fp : func_overload_set[name_index]) {
          PyObject *ret = fp(vm, args);
          if (ret) {
            return ret;
          }
        }
        vm->TypeError("no matching method found");
        return nullptr;
      };
      std::string sig = name;
      sig += "(*args)";
      vm->bind(mod, sig.c_str(), fp);
    }

    auto fp = [fn](VM *vm, ArgsView args) -> PyObject * {
      Tuple &pack = CAST(Tuple &, args[0]);
      args = ArgsView(pack.begin(), pack.end());
      return call_func(vm, fn, args,
                       std::make_index_sequence<sizeof...(Args)>{});
    };

    func_overload_set[name_index].push_back(fp);
    return *this;
  }

public:
  module_(VM *vm, PyObject *mod) : vm(vm), mod(mod) {}
};

template <typename... Args> class init {
public:
  init() = default;
};

template <typename T> class class_ {
  VM *vm;
  PyObject *mod;
  PyObject *cls;

  using Method = PyObject *(*)(VM *, ArgsView);
  inline static std::vector<Method> constructor_overload_set;

  using CXX_Method = std::function<PyObject *(VM *, ArgsView)>;
  inline static std::map<pkpy::StrName, std::vector<CXX_Method>>
      method_overload_set;

private:
  template <typename... Args, std::size_t... Is>
  static PyObject *gcnew(VM *vm, Type type, PyObject **args,
                         std::index_sequence<Is...>) {
    return vm->heap.gcnew<T>(type, py_cast<Args>(vm, args[Is])...);
  }

public:
  class_(module_ mod_, const char *name) : vm(mod_.vm), mod(mod_.mod) {
    cls = vm->new_type_object(mod, name, 0);
    mod->attr().set(name, cls);
    vm->_cxx_typeid_map[typeid(T)] = PK_OBJ_GET(Type, cls);
  }

  template <typename M> class_ &def_readwrite(const char *name, M T::*member) {
    auto fget = [member](VM *vm, ArgsView args) -> PyObject * {
      T &self = PK_OBJ_GET(T, args[0]);
      return py_var(vm, self.*member);
    };

    auto fset = [member](VM *vm, ArgsView args) -> PyObject * {
      T &self = PK_OBJ_GET(T, args[0]);
      self.*member = py_cast<M>(vm, args[1]);
      return vm->None;
    };

    vm->bind_property(cls, name, fget, fset);
    return *this;
  }

  template <typename... Args> class_ &def(init<Args...>) {
    if (constructor_overload_set.empty()) {
      auto fp = +[](VM *vm, ArgsView args) -> PyObject * {
        for (auto fp : constructor_overload_set) {
          PyObject *ret = fp(vm, args);
          if (ret) {
            return ret;
          }
        }
        vm->TypeError("no matching constructor found");
        return nullptr;
      };
      vm->bind(cls, "__new__(cls, *args)", fp);
    }

    auto fp = +[](VM *vm, ArgsView args) -> PyObject * {
      Type type = PK_OBJ_GET(Type, args[0]);
      Tuple &pack = CAST(Tuple &, args[1]);
      args = ArgsView(pack.begin(), pack.end());
      if (!check_cxx_types<Args...>(vm, args)) {
        return nullptr;
      } else {
        return gcnew<Args...>(vm, type, pack.begin(),
                              std::index_sequence_for<Args...>{});
      }
    };

    constructor_overload_set.push_back(fp);
    return *this;
  }

  // defualt argument has not been supported
  template <typename Ret, typename... Args, typename... Extras>
  class_ &def(const char *name, Ret (T::*member_fn)(Args...),
              const Extras &...extras) {
    Str name_index = name;
    if (method_overload_set.find(name_index) == method_overload_set.end()) {
      method_overload_set[name_index] = {};
      auto fp = [name_index](VM *vm, ArgsView args) -> PyObject * {
        for (auto fp : method_overload_set[name_index]) {
          PyObject *ret = fp(vm, args);
          if (ret) {
            return ret;
          }
        }
        vm->TypeError("no matching method found");
        return nullptr;
      };
      std::string sig = name;
      sig += "(self,*args)";
      vm->bind(cls, sig.c_str(), fp);
    }

    auto fp = [member_fn](VM *vm, ArgsView args) -> PyObject * {
      T &self = PK_OBJ_GET(T, args[0]);
      Tuple &pack = CAST(Tuple &, args[1]);
      args = ArgsView(pack.begin(), pack.end());
      return call_cxx_method(vm, &self, member_fn, args,
                             std::make_index_sequence<sizeof...(Args)>{});
    };

    method_overload_set[name_index].push_back(fp);
    return *this;
  }
};

template <typename T> class enum_ {
  VM *vm;
  PyObject *mod;
  PyObject *cls;
  const char *cls_name;

public:
  enum_(module_ mod_, const char *name)
      : vm(mod_.vm), mod(mod_.mod), cls_name(name) {
    cls = vm->new_type_object(mod, name, 0);
    mod->attr().set(name, cls);
    vm->_cxx_typeid_map[typeid(T)] = PK_OBJ_GET(Type, cls);
  }

  enum_ &value(const char *name, T value) {
    cls->attr().set(name,
                    py_var(vm, static_cast<std::underlying_type_t<T>>(value)));
    return *this;
  }
};
}; // namespace pkpy::cxx_bind