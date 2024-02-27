#include "pocketpy/include/pocketpy.h"

/**
TODO:
    * support function overload
    * support enum
    * support special function
*/

namespace pkpy {
struct module_ {
  VM *vm;
  PyObject *mod;
};

template <typename... Args> class init {
public:
  init() = default;
};

template <typename T> class class_ {
  VM *vm;
  PyObject *mod;
  PyObject *cls;

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
    auto fp = +[](VM *vm, ArgsView args) -> PyObject * {
      Type type = PK_OBJ_GET(Type, args[0]);
      Tuple &pack = CAST(Tuple &, args[1]);
      return gcnew<Args...>(vm, type, pack.begin(),
                            std::index_sequence_for<Args...>{});
    };
    vm->bind(cls, "__new__(cls, *args)", fp);
    return *this;
  }

  // defualt argument has not been supported
  template <typename Ret, typename... Args, typename... Extras>
  class_ &def(const char *name, Ret (T::*member_fn)(Args...),
              const Extras &...extras) {
    auto fp = [member_fn](VM *vm, ArgsView args) -> PyObject * {
      T &self = PK_OBJ_GET(T, args[0]);
      std::size_t Is = 0;
      if constexpr (std::is_void_v<Ret>) {
        (self.*member_fn)(py_cast<Args>(vm, args[Is += 1])...);
        return vm->None;
      } else {
        return py_var(vm,
                      (self.*member_fn)(py_cast<Args>(vm, args[Is += 1])...));
      }
    };

    std::string sig;
    sig.reserve(sizeof...(Args) * 4);
    sig += name;
    sig += "(self";
    for (std::size_t index = 0; index < sizeof...(Args); ++index) {
      sig += ",v";
      sig += std::to_string(index);
    }
    sig += ")";
    vm->bind(cls, sig.c_str(), fp);
    return *this;
  }
};

}; // namespace pkpy