#include "Stream.hpp"
#include <iostream>

template <typename Elem>
class RealTimeDeque {
private:
    static const int c = 2;
    int fr_sz=0, re_sz=0;
    Stream<Elem> front, rear, front_schedule, rear_schedule;
    static Stream<Elem> exec1(const Stream<Elem> s) {
        if (!s.isEmpty()) return s.tail();
        else return s;
    }
    static Stream<Elem> exec2(const Stream<Elem> s) {
        return exec1(exec1(s));
    }
    static Stream<Elem> rotateRev(const Stream<Elem> s, const Stream<Elem> r, const Stream<Elem> a) {
        if (s.isEmpty()) return r.rev() + a;
        else return Stream<Elem>::make_thunk([x=s.head(),f=s.tail(),r=r,c=c,a=a](){
                    return StreamCell<Elem>(x,rotateRev(f, r.drop(c), r.take(c).rev() + a));
                });
    }
    static Stream<Elem> rotateDrop(const Stream<Elem> f, int j, const Stream<Elem> r) {
        if (j < c) return rotateRev(f, r.drop(j), Stream<Elem>());
        else return Stream<Elem>::make_thunk([fp=f.tail(),x=f.head(),j=j,c=c,r=r](){
                    return StreamCell<Elem>(x, rotateDrop(fp, j - c, r.drop(c)));
                });
    }
    RealTimeDeque check() const {
        if (fr_sz > c*re_sz + 1) {
            int i = (fr_sz + re_sz) / 2;
            int j = fr_sz + re_sz - i;
            auto fp = front.take(i);
            auto rp = rotateDrop(rear, i, front);
            return RealTimeDeque(i,j,fp,rp,fp,rp);
        } else if (re_sz > c*fr_sz + 1) {
            int j = (fr_sz + re_sz) / 2;
            int i = fr_sz + re_sz - j;
            auto rp = rear.take(j);
            auto fp = rotateDrop(front, j, rear);
            return RealTimeDeque(i,j,fp,rp,fp,rp);
        } else {
            return *this;
        }
    }
public:
    RealTimeDeque() = default;
    RealTimeDeque(int fr_sz, int re_sz, Stream<Elem> front, Stream<Elem> rear, Stream<Elem> front_schedule, Stream<Elem> rear_schedule) 
    : fr_sz(fr_sz), re_sz(re_sz), front(front), rear(rear), front_schedule(front_schedule), rear_schedule(rear_schedule) {}
    static RealTimeDeque empty() {
        return RealTimeDeque();
    }
    bool isEmpty() const {
        return fr_sz + re_sz == 0;
    }
    int size() const {
        return fr_sz + re_sz;
    }
    RealTimeDeque cons(Elem x) const {
        return RealTimeDeque(fr_sz+1, re_sz, front.cons(x), rear, exec1(front_schedule), exec1(rear_schedule)).check();
    }
    friend RealTimeDeque cons(Elem x, const RealTimeDeque que) {
        return que.cons(x);
    }
    Elem head() const {
        if (front.isEmpty() && rear.isEmpty()) throw std::runtime_error("empty");
        else if (front.isEmpty()) return rear.head();
        else return front.head();
    }
    RealTimeDeque tail() const {
        if (front.isEmpty() && rear.isEmpty()) throw std::runtime_error("empty");
        else if (front.isEmpty()) return RealTimeDeque();
        else return RealTimeDeque(fr_sz-1, re_sz, front.tail(), rear, exec2(front_schedule), exec2(rear_schedule)).check();
    }
    RealTimeDeque snoc(Elem x) const {
        return RealTimeDeque(fr_sz, re_sz+1, front, rear.cons(x), exec1(front_schedule), exec1(rear_schedule)).check();
    }
    friend RealTimeDeque snoc(Elem x, const RealTimeDeque que) {
        return que.snoc(x);
    }
    Elem last() const {
        if (front.isEmpty() && rear.isEmpty()) throw std::runtime_error("empty");
        else if (rear.isEmpty()) return front.head();
        else return rear.head();
    }
    RealTimeDeque init() const {
        if (front.isEmpty() && rear.isEmpty()) throw std::runtime_error("empty");
        else if (rear.isEmpty()) return RealTimeDeque();
        else return RealTimeDeque(fr_sz, re_sz-1, front, rear.tail(), exec2(front_schedule), exec2(rear_schedule)).check();
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
