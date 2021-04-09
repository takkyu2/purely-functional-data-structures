#include "List.hpp"
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"
#include <variant>
#include <iostream>

template <typename Elem>
class HoodMelvilleRealTimeQueue {
private:
    struct Idle { };
    struct Reversing {
        Reversing() = default;
        Reversing(int ok, ImList<Elem> f1, ImList<Elem> f2, ImList<Elem> r1, ImList<Elem> r2) 
            : ok(ok), f1(f1), f2(f2), r1(r1), r2(r2) {}
        int ok;
        ImList<Elem> f1, f2, r1, r2;
    };
    struct Appending {
        Appending() = default;
        Appending(int ok, ImList<Elem> f, ImList<Elem> r)
            : ok(ok), f(f), r(r) {}
        int ok;
        ImList<Elem> f, r;
    };
    struct Done {
        Done() = default;
        Done(ImList<Elem> xs) : xs(xs) {}
        ImList<Elem> xs;
    };
    using RotationState = std::variant<Idle, Reversing, Appending, Done>;

    template <typename... Args>
    static RotationState make_reversing(Args&&... args) {
        return Reversing(std::forward<Args>(args)...);
    }
    template <typename... Args>
    static RotationState make_appending(Args&&... args) {
        return Appending(std::forward<Args>(args)...);
    }
    template <typename... Args>
    static RotationState make_done(Args&&... args) {
        return Done(std::forward<Args>(args)...);
    }
    int fr_sz=0, re_sz=0;
    ImList<Elem> front, rear;
    RotationState state;
    static RotationState exec(const RotationState state) {
        static auto exec_visitor = overloaded {
            [] (Reversing state) {
                if (state.f1.isEmpty()) {
                    return make_appending(state.ok, state.f2, cons(state.r1.head(), state.r2));
                } else {
                    return make_reversing(state.ok+1,
                            state.f1.tail(),
                            cons(state.f1.head(), state.f2),
                            state.r1.tail(),
                            cons(state.r1.head(), state.r2)
                            );
                }
            },
            [] (Appending state) {
                if (state.ok == 0) {
                    return make_done(state.r);
                } else {
                    return make_appending(state.ok-1, 
                            state.f.tail(),
                            cons(state.f.head(), state.r));
                }
            },
            [] (auto state) {
                return RotationState(state);
            }
        };
        return std::visit(exec_visitor, state);
    }
    static RotationState invalidate(const RotationState state) {
        static auto invalidate_visitor = overloaded {
            [] (Reversing state) {
                return make_reversing(state.ok-1, state.f1, state.f2, state.r1, state.r2);
            },
            [] (Appending state) {
                if (state.ok == 0) {
                    return make_done(state.r.tail());
                } else {
                    return make_appending(state.ok-1,
                            state.f,
                            state.r
                            );
                }
            },
            [] (auto state) {
                return RotationState(state);
            }
        };
        return std::visit(invalidate_visitor, state);
    }
    HoodMelvilleRealTimeQueue exec2() const {
        auto newst = exec(exec(state));
        if (std::holds_alternative<Done>(newst)) {
            return HoodMelvilleRealTimeQueue(fr_sz, re_sz, std::get<Done>(newst).xs,rear,Idle{});
        } else {
            return HoodMelvilleRealTimeQueue(fr_sz, re_sz, front, rear, newst);
        }
    }
    HoodMelvilleRealTimeQueue check() const {
        if (re_sz <= fr_sz) return exec2();
        else {
            auto newst = make_reversing(0,front,ImList<Elem>::empty(),rear,ImList<Elem>::empty());
            return HoodMelvilleRealTimeQueue(fr_sz+re_sz, 0, front, {}, newst).exec2();
        }
    }
public:
    // Strange error: If we use HoodMelvilleRealTimeQueue() = default here, the run-time behavior seems undefined.
    // I am not sure why (Maybe a bug of variant?).
    // Update: I found why: built-in types are not default initialized, so that fr_sz and re_sz are not 0, w/o =0 in the member declaration. Silly me...
    HoodMelvilleRealTimeQueue() = default;
    /* HoodMelvilleRealTimeQueue() : fr_sz(0), re_sz(0), front(), rear(), state() {} */
    HoodMelvilleRealTimeQueue(int fr_sz, int re_sz, ImList<Elem> front, ImList<Elem> rear, RotationState state)
        : fr_sz(fr_sz), re_sz(re_sz), front(front), rear(rear), state(state) {}
    static HoodMelvilleRealTimeQueue empty() {
        return HoodMelvilleRealTimeQueue();
    }
    bool isEmpty() const {
        return fr_sz == 0;
    }
    HoodMelvilleRealTimeQueue snoc(Elem x) const {
        return HoodMelvilleRealTimeQueue(fr_sz,re_sz+1,front, cons(x,rear), state).check();
    }
    Elem head() const {
        if (front.isEmpty()) throw std::runtime_error("Empty");
        else return front.head();
    }
    HoodMelvilleRealTimeQueue tail() const {
        if (front.isEmpty()) throw std::runtime_error("Empty");
        else return HoodMelvilleRealTimeQueue(fr_sz-1,re_sz,front.tail(),rear,invalidate(state)).check();
    }
    void printall() {
        std::cerr << fr_sz << " " << re_sz << std::endl;
        std::cerr << "front" << std::endl;
        auto tmp = front;
        while (!tmp.isEmpty()) {
            std::cerr << tmp.head() << std::endl;
            tmp = tmp.tail();
        }
        std::cerr << "rear" << std::endl;
        tmp = rear;
        while (!tmp.isEmpty()) {
            std::cerr << tmp.head() << std::endl;
            tmp = tmp.tail();
        }
        std::cerr << "state" << std::endl;
        if (std::holds_alternative<Idle>(state)) {
            std::cerr << "idle" << std::endl;
        } else if (std::holds_alternative<Reversing>(state)) {
            std::cerr << "reversing" << std::endl;
        } else if (std::holds_alternative<Appending>(state)) {
            std::cerr << "appending" << std::endl;
            
        } else {
            std::cerr << "done" << std::endl;
        }
    }
};
