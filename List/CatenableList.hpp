#ifndef CATENABLELIST
#define CATENABLELIST
#include "../utils/utils.hpp"
#include "RealTimeQueue.hpp"
#include <variant>

class Nil {};

template <typename Elem>
using Queue = RealTimeQueue<Elem>;

template <typename Elem, typename Basetype>
class CatenableList {
private:
    using Susplist = FunctionPtr<CatenableList<Elem, Basetype>()>;
    struct Cat {
        Elem elem;
        Queue<Susplist> m_cat;
    };
    std::variant<Nil, Cat> m_catlist;
    static CatenableList link(const CatenableList lhs, const Susplist rhs) {
        auto [x, q] = std::get<Cat>(lhs.m_catlist);
        return Cat(x, q.snoc(rhs));
    }
    static CatenableList linkAll(const Queue<Susplist> q) {
        Susplist t = q.head();
        Queue<Susplist> qp = q.tail();
        if (qp.isEmpty()) return t();
        else return link(t(), make_thunk([qp=qp](){return linkAll(qp);}));
    }

    template <typename F>
    static Susplist make_thunk(F&& lam) {
        return Susplist(make_lazy_val(std::forward<F>(lam)));
    }
public:
    CatenableList() = default;
    CatenableList(Cat c) : m_catlist(c) {}
    static CatenableList empty() {
        return CatenableList();
    }
    bool isEmpty() const {
        return std::holds_alternative<Nil>(m_catlist);
    }
    friend CatenableList operator+(CatenableList lhs, CatenableList rhs) {
        if (lhs.isEmpty()) return rhs;
        else if (rhs.isEmpty()) return lhs;
        else return link(lhs, make_thunk([rhs=rhs](){return rhs;}));
    }
    CatenableList cons(Elem x) const {
        return CatenableList(Cat(x, Queue<Susplist>())) + *this;
    }
    CatenableList snoc(Elem x) const {
        return *this + CatenableList(Cat(x, Queue<Susplist>()));
    }
    Elem head() const {
        if (isEmpty()) throw std::runtime_error("EMPTY");
        return std::get<Cat>(m_catlist).elem;
    }
    CatenableList tail() const {
        if (isEmpty()) throw std::runtime_error("EMPTY");
        auto q = std::get<Cat>(m_catlist).m_cat;
        if (q.isEmpty()) return CatenableList();
        else return linkAll(q);
    }
};

template <typename Elem>
using CatenableListBase = CatenableList<Elem, Elem>;
#endif
