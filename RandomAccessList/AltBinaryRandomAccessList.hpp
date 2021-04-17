#include <tuple>
#include <variant>
#include <memory>
#include <functional>
#include <type_traits>
#include "../utils/utils.hpp"

constexpr int cutoff = 10;

template <typename T>
class Printer;

struct Nil {};

template <typename Elem>
class AltBinaryRandomAccessList {
private:
    struct Zero {
        std::shared_ptr<AltBinaryRandomAccessList<nested_tuple<1,Elem>>> m_next = std::make_shared<AltBinaryRandomAccessList<nested_tuple<1,Elem>>>(AltBinaryRandomAccessList<nested_tuple<1,Elem>>(Nil()));
    };
    struct One {
        One() = default;
        One(Elem elem) 
            : elem(elem)
        {}
        One(Elem elem, std::shared_ptr<AltBinaryRandomAccessList<nested_tuple<1,Elem>>> seq) 
            : elem(elem), m_next(seq) {}
        Elem elem;
        std::shared_ptr<AltBinaryRandomAccessList<nested_tuple<1,Elem>>> m_next = std::make_shared<AltBinaryRandomAccessList<nested_tuple<1,Elem>>>(AltBinaryRandomAccessList<nested_tuple<1,Elem>>(Nil()));
    };
    std::variant<Nil, Zero, One> m_seq;
    static AltBinaryRandomAccessList make_nil() {
        return AltBinaryRandomAccessList();
    }
    template <typename... Args>
    static AltBinaryRandomAccessList make_zero(Args&&... args) {
        return Zero(args...);
    }
    template <typename... Args>
    static AltBinaryRandomAccessList make_one(Args&&... args) {
        return One(args...);
    }
    std::tuple<Elem, AltBinaryRandomAccessList> uncons() const {
        if constexpr (std::is_same_v<Elem, nested_tuple<cutoff,int>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return std::tuple<Elem, AltBinaryRandomAccessList>(); 
        } else {
            static auto visitor_uncons = overloaded {
                [] (Nil seq) {
                    throw std::runtime_error("EMPTY");
                    return std::tuple<Elem, AltBinaryRandomAccessList>();
                },
                [] (One ps) {
                    Elem x = ps.elem;
                    if (std::holds_alternative<Nil>(*(ps.m_next))) {
                        return std::make_tuple(x, make_nil());
                    } else {
                        return std::make_tuple(x, make_zero(ps.m_next));
                    }
                },
                [] (Zero ps) {
                    auto [xy, ps2] = ps.m_next->uncons();
                    auto [x, y] = xy;
                    return std::make_tuple(x, make_one(y, ps2->m_next));
                }
            };
            return std::visit(visitor_uncons, m_seq);
        }
    }
    template <typename F>
    AltBinaryRandomAccessList fupdate(F f, int i) {
        if constexpr (std::is_same_v<Elem, nested_tuple<cutoff,int>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return std::tuple<Elem, AltBinaryRandomAccessList>(); 
        } else {
            static auto visitor_fupdate = overloaded {
                [] (F f, int i, Nil seq) {
                    throw std::runtime_error("SUBSCRIPT");
                    return AltBinaryRandomAccessList();
                },
                [] (F f, int i, One ps) {
                    auto [x, ps2] = ps;
                    if (i == 0) return make_one(f(x), ps2);
                    else return make_zero(ps2).fupdate(f, i-1).cons(x);
                },
                [] (F f, int i, Zero ps) {
                    auto [ps2] = ps;
                    return make_zero(ps2->fupdate([i=i,f=f](nested_tuple<1,Elem> xy){
                                auto [x,y] = xy;
                                if (i % 2 == 0) return std::make_tuple(f(x), y);
                                else return std::make_tuple(x, f(y));
                }, i / 2));
                }
            };
            return std::visit(std::bind_front(visitor_fupdate, f, i), m_seq);
        }
    }
public:
    AltBinaryRandomAccessList() = default;
    AltBinaryRandomAccessList(const AltBinaryRandomAccessList& rhs) = default;
    AltBinaryRandomAccessList(Nil n) 
        : m_seq(n) {}
    AltBinaryRandomAccessList(Zero z) 
        : m_seq(z) {}
    AltBinaryRandomAccessList(One o) 
        : m_seq(o) {}
    static AltBinaryRandomAccessList empty() {
        return AltBinaryRandomAccessList();
    }
    bool isEmpty() const {
        return std::holds_alternative<Nil>(m_seq);
    }
    AltBinaryRandomAccessList cons(Elem x) const {
        if constexpr (std::is_same_v<Elem, nested_tuple<cutoff,int>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return AltBinaryRandomAccessList();
        } else  {
            static auto visitor_cons = overloaded {
                [] (Elem x, Nil seq) {
                    return make_one(x);
                },
                [] (Elem x, Zero ps) {
                    return make_one(x, ps.m_next);
                },
                [] (Elem x, One ps) {
                    return make_zero(std::make_shared<AltBinaryRandomAccessList<nested_tuple<1,Elem>>>(ps.m_next->cons(std::make_tuple(x, ps.elem))));
                }
            };
            return std::visit(std::bind_front(visitor_cons, x), m_seq);
        }
    }
    Elem head() const {
        auto [x, tmp] = uncons();
        return x;
    }
    AltBinaryRandomAccessList tail() const {
        auto [tmp, xs] = uncons();
        return xs;
    }
    Elem lookup(int i) const {
        if constexpr (std::is_same_v<Elem, nested_tuple<cutoff,int>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return Elem();
        } else  {
            static auto visitor_lookup = overloaded {
                [] (int i, Nil seq) {
                    throw std::runtime_error("SUBSCRIPT");
                    return Elem();
                },
                [] (int i, One ps) {
                    auto [x, ps2] = ps;
                    if (i == 0) {
                        return x;
                    } else {
                        return make_zero(ps2).lookup(i-1);
                    }
                },
                [] (int i, Zero ps) {
                    auto [x, y] = ps.m_next->lookup(i / 2);
                    if (i % 2 == 0) return x;
                    else return y;
                }
            };
            return std::visit(std::bind_front(visitor_lookup, i), m_seq);
        }
    }
    AltBinaryRandomAccessList update(int i, Elem y) {
        return fupdate([y](Elem x){return y;}, i);
    }
};
