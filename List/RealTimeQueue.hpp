#include "../List/Stream.hpp"
#include "../List/List.hpp"
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"

template <typename Elem>
class RealTimeQueue {
public:
    RealTimeQueue() = default;
    RealTimeQueue(Stream<Elem> fr, ImList<Elem> re, Stream<Elem> sch) 
        : front(fr), rear(re), schedule(sch) {}
    static RealTimeQueue empty() {
        return RealTimeQueue();
    }
    bool isEmpty() const {
        return front.isEmpty();
    }
    Stream<Elem> rotate() const {
        if (front.isEmpty()) return cons(rear.head(), schedule);
        else return cons(front.head(), RealTimeQueue(front.tail(), rear.tail(), cons(rear.head(), schedule)).rotate());
    }
    RealTimeQueue exec() const {
        if (schedule.isEmpty()) {
            auto newfr = rotate();
            return RealTimeQueue(newfr, ImList<Elem>::empty(), newfr);
        } else {
            return RealTimeQueue(front, rear, schedule.tail());
        }
    }
    RealTimeQueue snoc(Elem x) const {
        return RealTimeQueue(front, cons(x, rear), schedule).exec();
    }
    Elem head() const {
        if (isEmpty()) throw std::runtime_error("no elem");
        return front.head();
    }
    RealTimeQueue tail() const {
        if (isEmpty()) throw std::runtime_error("no elem");
        return RealTimeQueue(front.tail(), rear, schedule).exec();
    }
private:
    Stream<Elem> front;
    ImList<Elem> rear;
    Stream<Elem> schedule;
};
