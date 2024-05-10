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

    template <typename T, typename... Options>
    class class_ : public type
    {

    public:
        template <typename... Extras>
        class_(handle scope, const char* name, const Extras&... extras)
        {
            m_ptr = vm->new_type_object(scope.ptr(), name, vm->tp_object);
            vm->_cxx_typeid_map.insert({typeid(T), _builtin_cast<pkpy::Type>(m_ptr)});
        }
    };
}  // namespace pybind11
