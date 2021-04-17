#include "RealTimeDeque.hpp"
#include "../utils/utils.hpp"
#include <variant>

template <typename Elem>
using Deque = RealTimeDeque<Elem>;

constexpr int cutoff = 10;

template <size_t I,typename T> 
struct deque_depth_n {
    template<typename... Args> 
    using type = Deque<typename deque_depth_n<I-1, T>::template type<T, Args...>>;
};

template <typename T> 
struct deque_depth_n<0, T> {
    template <typename... Args>
    using type = T;
};

template <size_t I,typename T>  
using nested_deque = typename deque_depth_n<I,T>::template type<>;



template <typename Elem, typename Basetype>
class SimpleCatenableDeque {
private:
    using Suspdeque = FunctionPtr<SimpleCatenableDeque<Deque<Elem>, Basetype>()>;
    struct Shallow {
        Deque<Elem> m_shallow;
    };
    struct Deep {
        Deque<Elem> front;
        Suspdeque mid;
        Deque<Elem> rear;
    };
    std::variant<Shallow, Deep> m_deq;
    static bool tooSmall(Shallow d) {
        return d.m_shallow.isEmpty() || d.m_shallow.tail().isEmpty();
    }
    static bool tooSmall(Deque<Elem> d) {
        return d.isEmpty() || d.tail().isEmpty();
    }
    static Deque<Elem> dappendL(Deque<Elem> d1, Deque<Elem> d2) {
        if (d1.isEmpty()) return d2;
        else return d2.cons(d1.head());
    }
    static Deque<Elem> dappendL(Shallow d1, Shallow d2) {
        return dappendL(d1.m_shallow, d2.m_shallow);
    }
    static Deque<Elem> dappendR(Deque<Elem> d1, Deque<Elem> d2) {
        if (d2.isEmpty()) return d1;
        else return d1.cons(d2.head());
    }
    static Deque<Elem> dappendR(Shallow d1, Shallow d2) {
        return dappendR(d1.m_shallow, d2.m_shallow);
    }
    template <typename F>
    static Suspdeque make_thunk(F&& lam) {
        return Suspdeque(make_lazy_val(std::forward<F>(lam)));
    }

    template <typename... Args>
    static SimpleCatenableDeque make_shallow(Args&&... args) {
        return Shallow(std::forward<Args>(args)...);
    }
    template <typename... Args>
    static SimpleCatenableDeque make_deep(Args&&... args) {
        return Deep(std::forward<Args>(args)...);
    }

public:
    SimpleCatenableDeque() = default;
    SimpleCatenableDeque(Shallow s) : m_deq(s) {}
    SimpleCatenableDeque(Deep s) : m_deq(s) {}
    static SimpleCatenableDeque empty() {
        return SimpleCatenableDeque();
    }
    bool isEmpty() const {
        if constexpr (std::is_same_v<Elem, nested_deque<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return true;
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return std::get<Shallow>(m_deq).m_shallow.isEmpty();
            }
            return false;
        }
    }
    SimpleCatenableDeque cons(Elem x) const {
        if constexpr (std::is_same_v<Elem, nested_deque<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return SimpleCatenableDeque();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return Shallow(std::get<Shallow>(m_deq).m_shallow.cons(x));
            } else {
                auto [front, mid, rear] = std::get<Deep>(m_deq);
                return Deep(front.cons(x), mid, rear);
            }
        }
    }
    Elem head() const {
        if constexpr (std::is_same_v<Elem, nested_deque<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return Elem();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return std::get<Shallow>(m_deq).m_shallow.head();
            } else {
                auto [front, mid, rear] = std::get<Deep>(m_deq);
                return front.head();
            }
        }
    }
    SimpleCatenableDeque tail() const {
        if constexpr (std::is_same_v<Elem, nested_deque<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return SimpleCatenableDeque();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return Shallow(std::get<Shallow>(m_deq).m_shallow.tail());
            } else {
                auto [front, mid, rear] = std::get<Deep>(m_deq);
                auto fp = front.tail();
                if (!tooSmall(fp)) {
                    return Deep(fp, mid, rear);
                } else if (mid().isEmpty()) {
                    return Shallow(dappendL(fp, rear));
                } else {
                    return Deep(dappendL(fp, mid().head()), make_thunk([mid=mid](){return mid().tail();}), rear);
                }
            }
        }
    }
    SimpleCatenableDeque snoc(Elem x) const {
        if constexpr (std::is_same_v<Elem, nested_deque<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return SimpleCatenableDeque();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return Shallow(std::get<Shallow>(m_deq).m_shallow.snoc(x));
            } else {
                auto [front, mid, rear] = std::get<Deep>(m_deq);
                return Deep(front, mid, rear.snoc(x));
            }
        }
    }
    Elem last() const {
        if constexpr (std::is_same_v<Elem, nested_deque<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return Elem();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return std::get<Shallow>(m_deq).m_shallow.last();
            } else {
                auto [front, mid, rear] = std::get<Deep>(m_deq);
                return rear.last();
            }
        }
    }
    SimpleCatenableDeque init() const {
        if constexpr (std::is_same_v<Elem, nested_deque<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return SimpleCatenableDeque();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return Shallow(std::get<Shallow>(m_deq).m_shallow.init());
            } else {
                auto [front, mid, rear] = std::get<Deep>(m_deq);
                auto rp = rear.init();
                if (!tooSmall(rp)) {
                    return Deep(front, mid, rp);
                } else if (mid().isEmpty()) {
                    return Shallow(dappendR(front, rp));
                } else {
                    return Deep(front, make_thunk([mid=mid](){return mid().init();}), dappendR(mid().last(), rp));
                }
            }
        }
    }
    friend SimpleCatenableDeque operator+(const SimpleCatenableDeque lhs, const SimpleCatenableDeque rhs) {
        if constexpr (std::is_same_v<Elem, nested_deque<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return SimpleCatenableDeque();
        } else {
            static auto visiter_plus = overloaded {
                [](Shallow d1, Shallow d2) {
                    if (tooSmall(d1)) return make_shallow(dappendL(d1, d2));
                    else if (tooSmall(d2)) return make_shallow(dappendR(d1, d2));
                    else return make_deep(d1.m_shallow, make_thunk([](){return SimpleCatenableDeque<Deque<Elem>, Basetype>();}), d2.m_shallow);
                },
                [](Shallow d, Deep deep) {
                    auto [front, mid, rear] = deep;
                    if (tooSmall(d)) return make_deep(dappendL(d.m_shallow, front), mid, rear);
                    else return make_deep(d.m_shallow, make_thunk([front=front, mid=mid](){return mid().cons(front);}), rear);
                },
                [](Deep deep, Shallow d) {
                    auto [front, mid, rear] = deep;
                    if (tooSmall(d)) return make_deep(front, mid, dappendR(rear, d.m_shallow));
                    else return make_deep(front, make_thunk([rear=rear, mid=mid](){return mid().snoc(rear);}), d.m_shallow);
                },
                [](Deep d1, Deep d2) {
                    auto [f1,m1,r1] = d1;
                    auto [f2,m2,r2] = d2;
                    return make_deep(f1, make_thunk([m1=m1, r1=r1, m2=m2,f2=f2](){return m1().snoc(r1)+m2().cons(f2);}), r2);
                }
            };
            return std::visit(visiter_plus, lhs.m_deq, rhs.m_deq);
        }
    }
};

template <typename Elem>
using SimpleCatenableDequeBase = SimpleCatenableDeque<Elem, Elem>;
