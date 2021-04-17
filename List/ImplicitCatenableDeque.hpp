#include "RealTimeDeque.hpp"
#include "../utils/utils.hpp"
#include <variant>

template <typename Elem>
using Deque = RealTimeDeque<Elem>;

template <typename Elem, typename Basetype>
class ImplicitCatenableDeque;

constexpr int cutoff = 10;

template <typename Elem>
struct Simple {
    Deque<Elem> m_simple;
};

template <typename Elem, typename Basetype>
struct CmpdElem;

template <typename Elem, typename Basetype>
struct Cmpd {
    Deque<Elem> front;
    FunctionPtr<ImplicitCatenableDeque<CmpdElem<Elem, Basetype>, Basetype>()> mid;
    Deque<Elem> rear;
};

template <typename Elem, typename Basetype>
struct CmpdElem {
    std::variant<Simple<Elem>, Cmpd<Elem, Basetype>> m_cmpdelem;
};

template <size_t I,typename T, typename Basetype> 
struct cmpd_depth_n {
    template<typename... Args> 
    using type = CmpdElem<typename cmpd_depth_n<I-1, T, Basetype>::template type<T, Args...>, Basetype>;
};

template <typename T, typename Basetype> 
struct cmpd_depth_n<0, T, Basetype> {
    template <typename... Args>
    using type = T;
};

template <size_t I,typename T, typename Basetype>  
using nested_cmpd = typename cmpd_depth_n<I,T,Basetype>::template type<>;


template <typename Elem, typename Basetype>
class ImplicitCatenableDeque {
private:
    using Suspcmpd = FunctionPtr<ImplicitCatenableDeque<CmpdElem<Elem, Basetype>, Basetype>()>;
    struct Shallow {
        Deque<Elem> m_shallow;
    };
    struct Deep {
        Deque<Elem> f;
        Suspcmpd a;
        Deque<Elem> m;
        Suspcmpd b;
        Deque<Elem> r;
    };
    std::variant<Shallow, Deep> m_deq;

    static std::tuple<Deque<Elem>, Deque<Elem>, Deque<Elem>> share(Deque<Elem> f, Deque<Elem> r) {
        auto m = Deque<Elem>::empty().cons(r.head()).cons(f.last());
        return {f.init(), m, r.tail()};
    }
    static Deque<Elem> dappendL(Deque<Elem> d1, Deque<Elem> d2) {
        if (d1.isEmpty()) return d2;
        else return dappendL(d1.init(), d2.cons(d1.last()));
    }
    static Deque<Elem> dappendL(Shallow d1, Shallow d2) {
        return dappendL(d1.m_shallow, d2.m_shallow);
    }
    static Deque<Elem> dappendR(Deque<Elem> d1, Deque<Elem> d2) {
        if (d2.isEmpty()) return d1;
        else return dappendR(d1.snoc(d2.head()), d2.tail());
    }
    static Deque<Elem> dappendR(Shallow d1, Shallow d2) {
        return dappendR(d1.m_shallow, d2.m_shallow);
    }
    template <typename F>
    static Suspcmpd make_thunk(F&& lam) {
        return Suspdeque(make_lazy_val(std::forward<F>(lam)));
    }

    template <typename... Args>
    static ImplicitCatenableDeque make_shallow(Args&&... args) {
        return Shallow(std::forward<Args>(args)...);
    }
    template <typename... Args>
    static ImplicitCatenableDeque make_deep(Args&&... args) {
        return Deep(std::forward<Args>(args)...);
    }

public:
    ImplicitCatenableDeque() = default;
    ImplicitCatenableDeque(Shallow s) : m_deq(s) {}
    ImplicitCatenableDeque(Deep s) : m_deq(s) {}
    static ImplicitCatenableDeque empty() {
        return ImplicitCatenableDeque();
    }
    bool isEmpty() const {
        if constexpr (std::is_same_v<Elem, nested_cmpd<cutoff, Basetype, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return true;
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return std::get<Shallow>(m_deq).m_shallow.isEmpty();
            }
            return false;
        }
    }
    ImplicitCatenableDeque cons(Elem x) const {
        if constexpr (std::is_same_v<Elem, nested_cmpd<cutoff, Basetype, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return ImplicitCatenableDeque();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return Shallow(std::get<Shallow>(m_deq).m_shallow.cons(x));
            } else {
                auto [f, a, m, b, r] = std::get<Deep>(m_deq);
                return Deep(f.cons(x), a, m, b, r);
            }
        }
    }
    Elem head() const {
        if constexpr (std::is_same_v<Elem, nested_cmpd<cutoff, Basetype, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return Elem();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return std::get<Shallow>(m_deq).m_shallow.head();
            } else {
                auto [f, a, m, b, r] = std::get<Deep>(m_deq);
                return f.head();
            }
        }
    }
    ImplicitCatenableDeque snoc(Elem x) const {
        if constexpr (std::is_same_v<Elem, nested_cmpd<cutoff, Basetype, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return ImplicitCatenableDeque();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return Shallow(std::get<Shallow>(m_deq).m_shallow.snoc(x));
            } else {
                auto [f, a, m, b, r] = std::get<Deep>(m_deq);
                return Deep(f, a, m, b, r.snoc(x));
            }
        }
    }
    Elem last() const {
        if constexpr (std::is_same_v<Elem, nested_cmpd<cutoff,  Basetype,Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return Elem();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return std::get<Shallow>(m_deq).m_shallow.last();
            } else {
                auto [f, a, m, b, r] = std::get<Deep>(m_deq);
                return r.last();
            }
        }
    }
    friend ImplicitCatenableDeque operator+(const ImplicitCatenableDeque lhs, const ImplicitCatenableDeque rhs) {
        if constexpr (std::is_same_v<Elem, nested_cmpd<cutoff,  Basetype,Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return ImplicitCatenableDeque();
        } else {
            static auto visiter_plus = overloaded {
                [](Shallow d1, Shallow d2) {
                    if (d1.m_shallow.size() < 4) return make_shallow(dappendL(d1, d2));
                    else if (d2.m_shallow.size() < 4) return make_shallow(dappendR(d1, d2));
                    else {
                        auto [f,m,r] = share(d1.m_shallow, d2.m_shallow);
                        return make_deep(f, make_thunk([](){return ImplicitCatenableDeque<CmpdElem<Elem, Basetype>, Basetype>();}), m, make_thunk([](){return ImplicitCatenableDeque<CmpdElem<Elem, Basetype>, Basetype>();}), r);
                    }
                },
                [](Shallow d, Deep deep) {
                    auto [f, a, m, b, r] = deep;
                    if (d.m_shallow.size() < 4) return make_deep(dappendL(d.m_shallow, f), a, m, b, r);
                    else return make_deep(d.m_shallow, make_thunk([f=f, a=a](){return a().cons(Simple(f));}), m, b, r);
                },
                [](Deep deep, Shallow d) {
                    auto [f, a, m, b, r] = deep;
                    if (d.m_shallow.size() < 4) return make_deep(f,a,m,b,dappendR(r, d));
                    else return make_deep(f,a,m, make_thunk([b=b, r=r](){return b().cons(Simple(r));}), d);
                },
                [](Deep d1, Deep d2) {
                    auto [f1,a1,m1,b1,r1] = d1;
                    auto [f2,a2,m2,b2,r2] = d2;
                    auto [r1p, m, f2p] = share(r1,f2);
                    auto a1p = make_thunk([a1=a1, m1=m1,b1=b1, r1p=r1p](){return a1().snoc(Cmpd(m1,b1,r1p));});
                    auto b2p = make_thunk([f2p=f2p,a2=a2,m2=m2,b2=b2](){return b2().cons(Cmpd(f2p,a2,m2));});
                    return make_deep(f1, a1p, m, b2p, r2);
                }
            };
            return std::visit(visiter_plus, lhs.m_deq, rhs.m_deq);
        }
    }
    ImplicitCatenableDeque replaceHead(Elem x) const {
        if (std::holds_alternative<Shallow>(m_deq)) {
            return make_shallow(std::get<Shallow>(m_deq).m_shallow.tail().cons(x));
        } else {
            auto [f,a,m,b,r] = std::get<Deep>(m_deq);
            return make_deep(f.tail().cons(x), a, m, b, r);
        }
    }

    ImplicitCatenableDeque tail() const {
        if constexpr (std::is_same_v<Elem, nested_cmpd<cutoff, Basetype, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return ImplicitCatenableDeque();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return Shallow(std::get<Shallow>(m_deq).m_shallow.tail());
            } else {
                auto [f, a, m, b, r] = std::get<Deep>(m_deq);
                if (f.size() > 3) return make_deep(f.tail(),a,m,b,r);
                else if (!a().isEmpty()) {
                    auto hd = a().head();
                    if (std::holds_alternative<Simple>(hd)) {
                        auto fp = dappendL(f.tail(), std::get<Simple>(hd).m_simple);
                        return make_deep(fp, make_thunk([a=a](){return a().tail();}), m,b,r);
                    } else {
                        auto [fp, cp, rp] = std::get<Cmpd>(hd);
                        auto fpp = dappendL(f.tail(), fp);
                        auto app = make_thunk([cp=cp, rp=rp, a](){return cp() + replaceHead(Simple(rp), a());});
                        return make_deep(fpp, app, m, b, r);
                    }
                } else if (!b().isEmpty()) {
                    auto hd = b().head();
                    if (std::holds_alternative<Simple>(hd)) {
                        auto fp = dappendL(f.tail(), m);
                        return make_deep(fp,make_thunk([](){return ImplicitCatenableDeque<CmpdElem<Elem, Basetype>, Basetype>();}), std::get<Simple>(hd).m_simple, make_thunk([b=b](){return b().tail();}),r);
                    } else {
                        auto [fp, cp, rp] = std::get<Cmpd>(hd);
                        auto fpp = dappendL(f.tail(), m);
                        auto app = make_thunk([fp=fp,cp=cp](){return cp().cons(Simple(fp));});
                        return make_deep(fpp, app, rp, make_thunk([b=b](){return b().tail();}), r);
                    }
                } else {
                    return make_shallow(dappendL(f.tail(), m)) + make_shallow(r);
                }
            }
        }
    }
    ImplicitCatenableDeque replaceLast(Elem x) const {
        if (std::holds_alternative<Shallow>(m_deq)) {
            return make_shallow(std::get<Shallow>(m_deq).m_shallow.init().snoc(x));
        } else {
            auto [f,a,m,b,r] = std::get<Deep>(m_deq);
            return make_deep(f, a, m, b, r.init().snoc(x));
        }
    }

    ImplicitCatenableDeque init() const {
        if constexpr (std::is_same_v<Elem, nested_cmpd<cutoff, Basetype, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return ImplicitCatenableDeque();
        } else {
            if (std::holds_alternative<Shallow>(m_deq)) {
                return Shallow(std::get<Shallow>(m_deq).m_shallow.init());
            } else {
                auto [f, a, m, b, r] = std::get<Deep>(m_deq);
                if (r.size() > 3) return make_deep(f,a,m,b,r.init());
                else if (!b().isEmpty()) {
                    auto hd = b().last();
                    if (std::holds_alternative<Simple>(hd)) {
                        auto rp = dappendR(std::get<Simple>(hd).m_simple, r.init());
                        return make_deep(f, a, m,make_thunk([b=b](){return b().last();}),rp);
                    } else {
                        auto [fp, cp, rp] = std::get<Cmpd>(hd);
                        auto rpp = dappendR(rp, r.init());
                        auto bpp = make_thunk([cp=cp, fp=fp, b](){return cp() + replaceLast(b(), Simple(rp));});
                        return make_deep(f, a, m, bpp, rpp);
                    }
                } else if (!b().isEmpty()) {
                    auto hd = b().head();
                    if (std::holds_alternative<Simple>(hd)) {
                        auto fp = dappendL(f.tail(), m);
                        return make_deep(fp,make_thunk([](){return ImplicitCatenableDeque<CmpdElem<Elem, Basetype>, Basetype>();}), std::get<Simple>(hd).m_simple, make_thunk([b=b](){return b().tail();}),r);
                    } else {
                        auto [fp, cp, rp] = std::get<Cmpd>(hd);
                        auto fpp = dappendL(f.tail(), m);
                        auto app = make_thunk([fp=fp,cp=cp](){return cp().cons(Simple(fp));});
                        return make_deep(fpp, app, rp, make_thunk([b=b](){return b().tail();}), r);
                    }
                } else {
                    return make_shallow(dappendL(f.tail(), m)) + make_shallow(r);
                }
            }
        }
    }
};

template <typename Elem>
using ImplicitCatenableDequeBase = ImplicitCatenableDeque<Elem, Elem>;

