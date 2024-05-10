#include "cpp_function.h"

namespace pybind11
{
    class module_ : public object
    {
    public:
        using object::object;

        static module_ import(const char* name)
        {
            if(std::string_view(name) == "__main__")
            {
                return reinterpret_borrow<module_>(vm->_main);
            }
            else
            {
                return reinterpret_borrow<module_>(vm->py_import(name, false));
            }
        }
    };

    constexpr static auto _null_register = +[](pkpy::VM*, pkpy::PyObject*, pkpy::PyObject*) {};

    template <typename T, typename... Options>
    class class_ : public type
    {

    public:
        template <typename... Extras>
        class_(handle scope, const char* name, const Extras&... extras)
        {
            m_ptr = vm->register_user_class<T>(scope.ptr(), name, _null_register);
        }
    };
}  // namespace pybind11
