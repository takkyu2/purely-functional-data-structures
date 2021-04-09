#include "Stream.hpp"
#include <iostream>

template <typename Elem>
class BankersDeque {
private:
    static const int c = 2;
    int fr_sz=0, re_sz=0;
    Stream<Elem> front, rear;
public:
    BankersDeque() = default;
    BankersDeque(int fr_sz, int re_sz, Stream<Elem> front, Stream<Elem> rear) 
        : fr_sz(fr_sz), re_sz(re_sz), front(front), rear(rear) {}
    static BankersDeque empty() {
        return BankersDeque();
    }
    bool isEmpty() const {
        return fr_sz+re_sz == 0;
    }
    BankersDeque check() const {
        if (fr_sz > c*re_sz +1) {
            int i = (fr_sz+re_sz) / 2;
            int j = fr_sz+re_sz-i;
            return BankersDeque(
                    i,
                    j,
                    front.take(i),
                    rear + front.drop(i).rev()
                );
        } else if (re_sz > c*fr_sz + 1) {
            int j = (fr_sz+re_sz) / 2;
            int i = fr_sz+re_sz-j;
            return BankersDeque(
                    i,
                    j,
                    front + rear.drop(j).rev(),
                    rear.take(j)
                );
        } else {
            return *this;
        }
    }
    BankersDeque cons(Elem x) const {
        return BankersDeque(fr_sz+1, re_sz, front.cons(x), rear).check();
    }
    friend BankersDeque cons(Elem x, const BankersDeque que) {
        return que.cons(x);
    }
    BankersDeque snoc(Elem x) const {
        return BankersDeque(fr_sz, re_sz+1, front, rear.cons(x)).check();
    }
    friend BankersDeque snoc(Elem x, const BankersDeque que) {
        return que.snoc(x);
    }
    Elem head() const {
        if (front.isEmpty() && rear.isEmpty()) throw std::runtime_error("empty");
        else if (front.isEmpty()) return rear.head();
        else return front.head();
    }
    BankersDeque tail() const {
        if (front.isEmpty() && rear.isEmpty()) throw std::runtime_error("empty");
        else if (front.isEmpty()) return BankersDeque::empty();
        else return BankersDeque(fr_sz-1,re_sz,front.tail(), rear).check();
    }
    Elem last() const {
        if (front.isEmpty() && rear.isEmpty()) throw std::runtime_error("empty");
        else if (rear.isEmpty()) return front.head();
        else return rear.head();
    }
    BankersDeque init() const {
        if (front.isEmpty() && rear.isEmpty()) throw std::runtime_error("empty");
        else if (rear.isEmpty()) return BankersDeque::empty();
        else return BankersDeque(fr_sz,re_sz-1,front, rear.tail()).check();
    }
    void printall() {
        auto ftmp = front;
        std::cerr << "front" << std::endl;
        while (!ftmp.isEmpty()) {
            std::cerr << ftmp.head() << std::endl;
            std::cerr << "bef" << std::endl;
            ftmp = ftmp.tail();
            std::cerr << "aft" << std::endl;
        }
        std::cerr << "go" << std::endl;
        auto rtmp = rear;
        std::cerr << "rear" << std::endl;
        while (!rtmp.isEmpty()) {
            std::cerr << rtmp.head() << std::endl;
            rtmp = rtmp.tail();
        }
    }
};
