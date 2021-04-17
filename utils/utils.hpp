#ifndef UTILS_DEF
#define UTILS_DEF
#include <concepts>
#include <tuple>

struct Empty {};

template <typename T>
concept Ordered = requires(T a, T b) {
    {a < b} -> std::convertible_to<bool>;
    {a == b} -> std::convertible_to<bool>;
    {a <= b} -> std::convertible_to<bool>;
};

// use inheritance to overload operator()
template <typename... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

// type deduction guide
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <size_t I,typename T> 
struct tuple_depth_n{
    template<typename... Args>
    using type_each = typename tuple_depth_n<I-1, T>::template type<T, Args...>;

    template<typename... Args> 
    using type = std::tuple<type_each<Args...>, type_each<Args...>>;
};

template <typename T> 
struct tuple_depth_n<0, T> {
    template <typename... Args>
    using type = T;
};

// When the name of a member template specialization appears after . or -> in a postfix-expression, or after nested-name-specifier in a qualified-id, and the postfix-expression or qualified-id explicitly depends on a template-parameter (14.6.2), the member template name must be prefixed by the keyword template. Otherwise the name is assumed to name a non-template.
template <size_t I,typename T>  
using nested_tuple = typename tuple_depth_n<I,T>::template type<>;

#endif
