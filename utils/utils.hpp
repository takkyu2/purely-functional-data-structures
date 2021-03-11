#ifndef UTILS_DEF
#define UTILS_DEF
#include <concepts>

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

#endif
