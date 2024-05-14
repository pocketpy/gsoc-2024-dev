#include <tuple>
#include <type_traits>

namespace pybind11
{

    template <typename T>
    constexpr bool dependent_false = false;

    template <typename Fn>
    struct function_traits
    {
        static_assert(dependent_false<Fn>, "unsupported function type");
    };

#define PYBIND11_FUNCTION_TRAITS_SPECIALIZE(qualifiers)                                            \
    template <typename R, typename... Args>                                                        \
    struct function_traits<R(Args...) qualifiers>                                                  \
    {                                                                                              \
        using return_type = R;                                                                     \
        using args_type = std::tuple<Args...>;                                                     \
        constexpr static std::size_t args_count = sizeof...(Args);                                 \
    };

    PYBIND11_FUNCTION_TRAITS_SPECIALIZE()
    PYBIND11_FUNCTION_TRAITS_SPECIALIZE(&)
    PYBIND11_FUNCTION_TRAITS_SPECIALIZE(const)
    PYBIND11_FUNCTION_TRAITS_SPECIALIZE(const&)
    PYBIND11_FUNCTION_TRAITS_SPECIALIZE(noexcept)
    PYBIND11_FUNCTION_TRAITS_SPECIALIZE(& noexcept)
    PYBIND11_FUNCTION_TRAITS_SPECIALIZE(const noexcept)
    PYBIND11_FUNCTION_TRAITS_SPECIALIZE(const& noexcept)

#undef PYBIND11_FUNCTION_TRAITS_SPECIALIZE

    template <typename T>
    using function_return_t = typename function_traits<T>::return_type;

    template <typename T>
    using function_args_t = typename function_traits<T>::args_type;

    template <typename T>
    constexpr std::size_t function_args_count = function_traits<T>::args_count;

    template <typename T>
    struct member_traits;

    template <typename M, typename C>
    struct member_traits<M C::*>
    {
        using member_type = M;
        using class_type = C;
    };

    template <typename T>
    using member_type_t = typename member_traits<T>::member_type;

    template <typename T>
    using class_type_t = typename member_traits<T>::class_type;

    template <typename T, typename = decltype(&T::operator())>
    struct is_functor : std::true_type
    {
    };

    template <typename T>
    struct is_functor<T, void> : std::false_type
    {
    };

    template <typename T>
    constexpr bool is_functor_v = is_functor<T>::value;

    template <typename T>
    using functor_return_t = function_return_t<member_type_t<decltype(&T::operator())>>;

    template <typename T>
    using functor_args_t = function_args_t<member_type_t<decltype(&T::operator())>>;

    template <typename T>
    constexpr std::size_t functor_args_count =
        function_args_count<member_type_t<decltype(&T::operator())>>;

    using std::is_member_function_pointer_v;

    template <typename T>
    constexpr inline bool is_function_pointer_v = std::is_function_v<std::remove_pointer_t<T>>;

}  // namespace pybind11
