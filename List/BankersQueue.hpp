#ifndef BANKERSQUEUE
#define BANKERSQUEUE
#include "Stream.hpp"
#include <iostream>

template <typename Elem>
class BankersQueue {
public:
    BankersQueue() = default;
    BankersQueue(const Stream<Elem>& fr, const Stream<Elem>& re, int fr_sz, int re_sz) 
        : front(fr), rear(re), fr_sz(fr_sz), re_sz(re_sz) { }
    bool isEmpty() const {
        return fr_sz == 0 && re_sz == 0;
    }
    Elem head() const {
        if (fr_sz == 0) 
            throw std::runtime_error("empty");
        else 
            return front.head();
    }
    BankersQueue tail() const {
        if (fr_sz == 0) 
            throw std::runtime_error("empty");
        else 
            return BankersQueue(front.tail(), rear, fr_sz-1, re_sz).check();
    }
    friend BankersQueue snoc(const Elem elem, const BankersQueue que) {
        return BankersQueue(que.front, cons(elem, que.rear), que.fr_sz, que.re_sz+1).check();
    }
    BankersQueue snoc(Elem elem) const {
        return snoc(elem, *this);
    }
private:
    BankersQueue check() const {
        if (re_sz <= fr_sz) {
            return *this;
        } else {
            return BankersQueue(front + rear.rev(), Stream<Elem>(), fr_sz+re_sz+1,0);
        }
    }
    Stream<Elem> front, rear;
    int fr_sz = 0, re_sz = 0;
};

#endif
