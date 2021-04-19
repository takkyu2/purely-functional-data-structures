#ifndef IMPLICITQUEUE
#define IMPLICITQUEUE
#include <variant>
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"

constexpr int cutoff = 10;

template <typename Elem, typename Basetype>
class ImplicitQueue {
private:
    struct Zero {};
    struct One {
        Elem elem;
    };
    struct Two {
        Two(nested_tuple<1, Elem> elem) : elem(elem) {}
        Two(Elem x, Elem y) : elem(std::make_tuple(x,y)) {}
        nested_tuple<1, Elem> elem;
    };
    using Digit = std::variant<Zero, One, Two>;
    struct Shallow {
        Digit m_digit;
    };
    using Suspqueue = FunctionPtr<ImplicitQueue<nested_tuple<1, Elem>, Basetype>()>;
    struct Deep {
        Digit front_digit;
        Suspqueue m_que;
        Digit rear_digit;
    };
    std::variant<Shallow, Deep> m_que;
    template <typename F>
    static Suspqueue make_thunk(F&& lam) {
        return Suspqueue(make_lazy_val(std::forward<F>(lam)));
    }
public:
    ImplicitQueue() = default;
    ImplicitQueue(Shallow s) : m_que(s) {}
    ImplicitQueue(Deep s) : m_que(s) {}
    static ImplicitQueue empty() {
        return ImplicitQueue();
    }
    bool isEmpty() const {
        if constexpr (std::is_same_v<Elem, nested_tuple<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return true;
        } else {
            if (std::holds_alternative<Shallow>(m_que)) {
                if (std::holds_alternative<Zero>(std::get<Shallow>(m_que).m_digit)) return true;
            }
            return false;
        }
    }
    ImplicitQueue snoc(Elem y) const {
        if constexpr (std::is_same_v<Elem, nested_tuple<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return ImplicitQueue();
        } else {
            if (std::holds_alternative<Shallow>(m_que)) {
                auto shallow_digit = std::get<Shallow>(m_que).m_digit;
                if (std::holds_alternative<Zero>(shallow_digit)) {
                    return Shallow(One(y));
                } else {
                    return Deep(Digit(Two(std::get<One>(shallow_digit).elem, y)), make_thunk([](){return ImplicitQueue<nested_tuple<1, Elem>, Basetype>();}), Digit(Zero()));
                }
            } else {
                auto [f, q, r] = std::get<Deep>(m_que);
                if (std::holds_alternative<Zero>(r)) {
                    return Deep(f, q, Digit(One(y)));
                } else {
                    return Deep(f, make_thunk([y=y,q=q, x=std::get<One>(r).elem](){return q().snoc({x,y});}), Digit(Zero()));
                }
            }
        }
    }
    Elem head() const {
        if constexpr (std::is_same_v<Elem, nested_tuple<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return Elem();
        } else {
            if (std::holds_alternative<Shallow>(m_que)) {
                auto shallow_digit = std::get<Shallow>(m_que).m_digit;
                if (std::holds_alternative<Zero>(shallow_digit)) {
                    throw std::runtime_error("EMPTY");
                } else {
                    return std::get<One>(shallow_digit).elem;
                }
            } else {
                auto [f, q, r] = std::get<Deep>(m_que);
                if (std::holds_alternative<One>(f)) {
                    return std::get<One>(f).elem;
                } else {
                    auto xy = std::get<Two>(f).elem;
                    auto [x,y] = xy;
                    return x;
                }
            }
        }
    }
    ImplicitQueue tail() const {
        if constexpr (std::is_same_v<Elem, nested_tuple<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return ImplicitQueue();
        } else {
            if (std::holds_alternative<Shallow>(m_que)) {
                auto shallow_digit = std::get<Shallow>(m_que).m_digit;
                if (std::holds_alternative<Zero>(shallow_digit)) {
                    throw std::runtime_error("EMPTY");
                } else {
                    return ImplicitQueue();
                }
            } else {
                auto [f, q, r] = std::get<Deep>(m_que);
                if (std::holds_alternative<One>(f)) {
                    if (q().isEmpty()) return Shallow(r);
                    else {
                        auto yz = q().head();
                        auto [y, z] = yz;
                        return Deep(Digit(Two(y,z)), [q=q](){return q().tail();}, r);
                    }
                } else {
                    auto xy = std::get<Two>(f).elem;
                    auto [x,y] = xy;
                    return Deep(One(y), q, r);
                }
            }
        }
    }
};

template <typename Elem>
using ImplicitQueueBase = ImplicitQueue<Elem, Elem>;
#endif
