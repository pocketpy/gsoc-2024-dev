#include "builtins.h"

namespace pybind11
{
    template <typename T, typename = void>
    struct is_functor : std::false_type
    {
    };

    template <typename T>
    struct is_functor<T, std::void_t<decltype(&T::operator())>> : std::true_type
    {
    };

    template <typename T>
    constexpr inline bool is_functor_v = is_functor<T>::value;

    template <typename Fn>
    struct generate_helper;

    template <typename Ret, typename C, typename... Args>
    struct generate_helper<Ret (C::*)(Args...) const>
    {
        static pkpy::PyObject* wrapper(pkpy::VM* vm, pkpy::ArgsView view)
        {
            auto& fn = pkpy::lambda_get_userdata<C>(view.begin());
            std::size_t index = 0;
            auto& args = _builtin_cast<pkpy::Tuple>(view[0]);
            auto& kwargs = _builtin_cast<pkpy::Dict>(view[1]);
            return _cast(fn(cast<Args>(args[index++])...)).ptr();
        }
    };

    template <typename Fn>
    auto generate_wrapper()
    {
        if constexpr(is_functor_v<Fn>)
        {
            return generate_helper<decltype(&Fn::operator())>::wrapper;
        }
        else
        {
            static_assert(is_functor_v<Fn>, "Unsupported function type");
        }
    }

    class cpp_function : public function
    {
    public:
        template <typename Fn, typename... Extras>
        cpp_function(Fn&& fn, const Extras&... extras)
        {
            auto wrapper = generate_wrapper<Fn>();
            m_ptr = vm->bind(nullptr, "f(*args, **kwargs)", wrapper, std::forward<Fn>(fn));
        }
    };
}  // namespace pybind11
