#include "List.hpp"
#include <iostream>

template <typename Elem>
class Deque {
public:
    Deque() = default;
    Deque(const ImList<Elem>& fr, const ImList<Elem>& re, int fr_sz, int re_sz) 
        : front(fr), rear(re), fr_sz(fr_sz), re_sz(re_sz) { }
    bool isEmpty() const {
        return fr_sz == 0 && re_sz == 0;
    }
    friend Deque cons(const Elem& elem, Deque que) {
        return Deque(cons(elem, que.front), que.rear, que.fr_sz+1, que.re_sz);
    }
    Elem head() const {
        if (isEmpty()) 
            throw std::runtime_error("empty");
        if (fr_sz == 0) 
            return rear.head();
        else 
            return front.head();
    }
    Deque tail() const {
        if (isEmpty()) 
            throw std::runtime_error("empty");
        if (fr_sz == 0) 
            return Deque();
        else 
            return Deque(front.tail(), rear, fr_sz-1, re_sz).check_front();
    }
    friend Deque snoc(const Elem& elem, Deque que) {
        return Deque(que.front, cons(elem, que.rear), que.fr_sz, que.re_sz+1);
    }
    Elem last() {
        if (isEmpty()) 
            throw std::runtime_error("empty");
        if (re_sz == 0) 
            return front.head();
        else 
            return rear.head();
    }
    Deque init() {
        if (isEmpty()) 
            throw std::runtime_error("empty");
        if (re_sz == 0) 
            return Deque();
        else 
            return Deque(front, rear.tail(), fr_sz, re_sz-1).check_rear();
    }
private:
    Deque check_front() const {
        if (fr_sz == 0) {
            ImList<Elem> newre = ImList<Elem>::empty(), newfr = rear;
            for (int i = 0; i < re_sz/2; ++i) {
                newre = newre.cons(newfr.head());
                newfr = newfr.tail();
            }
            return Deque(newfr.rev(), newre.rev(), re_sz-re_sz/2, re_sz/2);
        }
        return *this;
    }
    Deque check_rear() const {
        if (re_sz == 0) {
            ImList<Elem> newfr = ImList<Elem>::empty(), newre = front;
            for (int i = 0; i < fr_sz/2; ++i) {
                newfr = newfr.cons(newre.head());
                newre = newre.tail();
            }
            return Deque(newfr.rev(), newre.rev(), fr_sz/2, fr_sz-fr_sz/2);
        }
        return *this;
    }
    ImList<Elem> front, rear;
    int fr_sz = 0, re_sz = 0;
};


int main() {
    Deque<int> que;
    for (int i = 0; i < 10; ++i) {
        que = cons(i, que);
    }
    for (int i = 20; i < 30; ++i) {
        que = snoc(i, que);
    }
    Deque<int> que2 = que;
    for (int i = 0; i < 20; ++i) {
        std::cout << que.head() << std::endl;
        que = que.tail();
    }
    std::cout << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < 20; ++i) {
        std::cout << que2.last() << std::endl;
        que2 = que2.init();
    }
}
