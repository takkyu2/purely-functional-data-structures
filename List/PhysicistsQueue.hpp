#ifndef PHYSICISTSQUEUE
#define PHYSICISTSQUEUE
#include "Stream.hpp"
#include "List.hpp"
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"
#include <iostream>

template <typename Elem>
class ListSusp {
private:
    std::shared_ptr<FunctionPtr<ImList<Elem>()>> m_list;
public:
    ListSusp() {
        *this = make_thunk([](){return ImList<Elem>();});
    }
    ListSusp(FunctionPtr<ImList<Elem>()> f)
        : m_list(std::make_shared<FunctionPtr<ImList<Elem>()>>(f)) { }
    ImList<Elem> force() const {
        return (*m_list)();
    }
    ListSusp tail() const {
        return make_thunk([this_list=*this]() {
                return this_list.force().tail();
        });
    }
    static ListSusp comb(ImList<Elem> lhs, ImList<Elem> rhs) {
        return make_thunk([lhs, rhs]() {
                return lhs + rhs;
        });
    }
private:
    template <typename F>
    static ListSusp make_thunk(F&& lam) {
        return ListSusp(make_lazy_val(std::forward<F>(lam)));
    }
};

template <typename Elem>
class PhysicistsQueue {
public:
    PhysicistsQueue() = default;
    PhysicistsQueue(const ImList<Elem>& w, const ListSusp<Elem>& fr, const ImList<Elem>& re, int fr_sz, int re_sz) 
        : w(w), front(fr), rear(re), fr_sz(fr_sz), re_sz(re_sz) { }
    bool isEmpty() const {
        return fr_sz == 0;
    }
    Elem head() const {
        if (w.isEmpty()) 
            throw std::runtime_error("empty");
        else 
            return w.head();
    }
    PhysicistsQueue tail() const {
        if (w.isEmpty()) 
            throw std::runtime_error("empty");
        else 
            return PhysicistsQueue(w.tail(), front.tail(), rear, fr_sz-1, re_sz).check();
    }
    friend PhysicistsQueue snoc(const Elem elem, const PhysicistsQueue que) {
        return PhysicistsQueue(que.w, que.front, cons(elem, que.rear), que.fr_sz, que.re_sz+1).check();
    }
    PhysicistsQueue snoc(Elem elem) const {
        return snoc(elem, *this);
    }
private:
    PhysicistsQueue check() const {
        if (re_sz <= fr_sz)
            return checkw();
        else {
            auto fr_forced = front.force();
            return PhysicistsQueue(fr_forced, ListSusp<Elem>::comb(fr_forced, rear.rev()), ImList<Elem>(),fr_sz+re_sz,0).checkw();
        }
    }
    PhysicistsQueue checkw() const {
        if (w.isEmpty())
            return PhysicistsQueue(front.force(), front, rear, fr_sz, re_sz);
        else
            return *this;
    }
    ImList<Elem> w;
    ListSusp<Elem> front;
    ImList<Elem> rear;
    int fr_sz = 0, re_sz = 0;
};

#endif
