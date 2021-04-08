#include "Stream.hpp"
#include <iostream>

template <typename Elem>
class Queue {
public:
    Queue() = default;
    Queue(const Stream<Elem>& fr, const Stream<Elem>& re, int fr_sz, int re_sz) 
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
    Queue tail() const {
        if (fr_sz == 0) 
            throw std::runtime_error("empty");
        else 
            return Queue(front.tail(), rear, fr_sz-1, re_sz).check();
    }
    friend Queue snoc(const Elem elem, const Queue que) {
        return Queue(que.front, cons(elem, que.rear), que.fr_sz, que.re_sz+1).check();
    }
    Queue snoc(Elem elem) const {
        return snoc(elem, *this);
    }
private:
    Queue check() const {
        if (re_sz <= fr_sz) {
            return *this;
        } else {
            return Queue(front + rear.rev(), Stream<Elem>(), fr_sz+re_sz+1,0);
        }
    }
    Stream<Elem> front, rear;
    int fr_sz = 0, re_sz = 0;
};


int main() {
    Queue<int> que;
    for (int i = 0; i < 30000; ++i) {
        que = snoc(i, que);
    }
    for (int i = 0; i < 10; ++i) {
        std::cout << que.head() << std::endl;
        que = que.tail();
    }
}

