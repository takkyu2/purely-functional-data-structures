#ifndef BOOTSTRAPPEDQUEUE
#define BOOTSTRAPPEDQUEUE
#include "../List/List.hpp"
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"
#include <variant>
#include <iostream>

constexpr int cutoff = 20;

template <size_t I,typename T> 
struct fptr_depth_n{
    template<typename... Args> 
    using type = FunctionPtr<ImList<typename fptr_depth_n<I-1,T>::template type<T, Args...>>()>;
};

template <typename T> 
struct fptr_depth_n<0, T> {
    template <typename... Args>
    using type = T;
};

template <size_t I,typename T>  
using nested_fptr = typename fptr_depth_n<I,T>::template type<>;

template <typename T>
class Printer;

class Nil {};

template <typename Elem, typename Basetype>
class BootstrappedQueue {
private:
    using Susplist = FunctionPtr<ImList<Elem>()>;
    struct SubQueue {
        int lenfm = 0;
        ImList<Elem> front;
        std::shared_ptr<BootstrappedQueue<Susplist, Basetype>> mid
            = std::make_shared<BootstrappedQueue<Susplist, Basetype>>(BootstrappedQueue<Susplist, Basetype>());
        int lenr = 0;
        ImList<Elem> rear;
    };
    std::shared_ptr<SubQueue> m_que = nullptr;

    template <typename F>
    static Susplist make_thunk(F&& lam) {
        return Susplist(make_lazy_val(std::forward<F>(lam)));
    }
public:
    BootstrappedQueue() = default;
    BootstrappedQueue(SubQueue que) : m_que(std::make_shared<SubQueue>(que)) {}
    static BootstrappedQueue empty() {
        return BootstrappedQueue();
    }
    bool isEmpty() const {
        return m_que == nullptr;
    }
    static BootstrappedQueue checkQ(const SubQueue que) {
        if constexpr (std::is_same_v<Elem, nested_fptr<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return BootstrappedQueue();
        } else {
            if (que.lenr <= que.lenfm) return checkF(que);
            else return checkF(SubQueue(
                        que.lenfm+que.lenr,
                        que.front,
                        std::make_shared<BootstrappedQueue<Susplist, Basetype>>(que.mid->snoc(make_thunk([rear=que.rear](){
                                return rear.rev();
                            }))),
                        0,
                        ImList<Elem>()
                        ));
        }
    }
    static BootstrappedQueue checkF(const SubQueue que) {
        if constexpr (std::is_same_v<Elem, nested_fptr<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return BootstrappedQueue();
        } else {
            if (que.front.isEmpty()) {
                if (que.mid->isEmpty()) {
                    return BootstrappedQueue();
                } else {
                    return SubQueue(
                            que.lenfm, 
                            que.mid->head()(),
                            std::make_shared<BootstrappedQueue<Susplist, Basetype>>(que.mid->tail()),
                            que.lenr,
                            que.rear
                            );
                }
            } else {
                return que;
            }
        }
    }
    BootstrappedQueue snoc(Elem x) const {
        if constexpr (std::is_same_v<Elem, nested_fptr<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return BootstrappedQueue();
        } else {
            if (isEmpty()) return SubQueue(1, cons(x, ImList<Elem>()), std::make_shared<BootstrappedQueue<Susplist, Basetype>>(BootstrappedQueue<Susplist, Basetype>()), 0, ImList<Elem>());
            else {
                auto que = *m_que;
                return checkQ(SubQueue(que.lenfm, que.front, que.mid, que.lenr+1, que.rear.cons(x)));
            }
        }
    }
    Elem head() const {
        if constexpr (std::is_same_v<Elem, nested_fptr<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return Elem();
        } else {
            if (isEmpty()) throw std::runtime_error("EMPTY");
            return (*m_que).front.head();
        }
    }
    BootstrappedQueue tail() const {
        if constexpr (std::is_same_v<Elem, nested_fptr<cutoff, Basetype>>) {
            throw std::runtime_error("CUTOFF exceeded!");
            return BootstrappedQueue();
        } else {
            if (isEmpty()) throw std::runtime_error("EMPTY");
            auto que = *m_que;
            return checkQ(SubQueue(que.lenfm-1, que.front.tail(), que.mid, que.lenr, que.rear));
        }
    }
};

template <typename Elem>
using BootstrappedQueueBase = BootstrappedQueue<Elem, Elem>;
#endif
