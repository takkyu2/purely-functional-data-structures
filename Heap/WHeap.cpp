#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <stack>
#include "../utils/utils.hpp"

using namespace std::placeholders;

template <Ordered Elem>
class Heap {
private:
    struct SubHeap {
        SubHeap(Elem x) : rank(1), elem(x) { }
        SubHeap(int rk, Elem x, std::shared_ptr<Heap> l, std::shared_ptr<Heap> r) 
            : rank(rk), elem(x), left(l), right(r) { }
        int rank;
        Elem elem;
        std::shared_ptr<Heap> left, right;
    };
public:
    Heap() = default;
    Heap(Empty e) { }
    Heap(SubHeap t) : m_heap(t) { }
    Heap(const Heap& bst) = default;
    static Heap Heapify(std::initializer_list<Elem> il) {
        if (il.size() == 0) return {};
        std::stack<Heap> now_st, next_st;
        for (Elem e: il) now_st.push(e);
        while (now_st.size() > 1) {
            while (now_st.size() >= 2) {
                Heap h1 = now_st.top(); now_st.pop();
                Heap h2 = now_st.top(); now_st.pop();
                next_st.push(h1.merge(h2));
            }
            if (now_st.size() == 1) {
                Heap h1 = now_st.top(); now_st.pop();
                next_st.push(h1);
            }
            swap(now_st, next_st);
        }
        return now_st.top();
    }
    Heap(Elem x)
        : m_heap(SubHeap(
                    1,
                    x,
                    std::make_shared<Heap>(Heap::empty()), 
                    std::make_shared<Heap>(Heap::empty())
                )
                ) { }
    Heap(int rank, Elem x, std::shared_ptr<Heap> l, std::shared_ptr<Heap> r)
        : m_heap(SubHeap(rank, x, l, r)) { }
    Heap(int rank, Elem x, Heap l, Heap r)
        : m_heap(SubHeap(rank, x, std::make_shared<Heap>(l), std::make_shared<Heap>(r))) { }
    static Heap empty() { return {};}
    Heap merge(Heap h) const {
        static auto visit_merge = overloaded{
            [] (Empty e, Empty x) {
                return Heap(x);
            },
            [] (Empty e, auto x) {
                return Heap(x);
            },
            [] (auto x, Empty e) {
                return Heap(x);
            },
            [] (SubHeap h1, SubHeap h2) {
                if (h1.elem < h2.elem)
                    return makeT(h1.elem, h1.left, std::make_shared<Heap>(h1.right->merge(h2)));
                else
                    return makeT(h2.elem, h2.left, std::make_shared<Heap>(h2.right->merge(h1)));
            },
        };
        return std::visit(visit_merge, m_heap, h.m_heap);
    }
    friend Heap merge_iterative(Heap hl, Heap hr) {
        auto head = std::make_shared<Heap>(Heap::empty());
        auto now = head;
        auto nowhl = std::make_shared<Heap>(hl), nowhr = std::make_shared<Heap>(hr);
        while (true) {
            if (nowhl->isEmpty()) {
                *now = *nowhr;
                break;
            }
            if (nowhr->isEmpty()) {
                *now = *nowhl;
                break;
            }
            int nowrank = nowhl->rank() + nowhr->rank() + 1;
            if (nowhl->getElem() <= nowhr->getElem()) {
                if (nowhl->getLeft()->rank() >= nowhl->getRight()->rank() + nowhr->rank()) {
                    *now = Heap(nowrank, nowhl->getElem(),  nowhl->getLeft(), std::make_shared<Heap>(Heap::empty()));
                    now = now->getRight();
                    nowhl = nowhl->getRight();
                } else {
                    *now = Heap(nowrank, nowhl->getElem(), std::make_shared<Heap>(Heap::empty()), nowhl->getLeft());
                    now = now->getLeft();
                    nowhl = nowhl->getRight();
                }
            } else {
                if (nowhr->getLeft()->rank() >= nowhr->getRight()->rank() + nowhl->rank()) {
                    *now = Heap(nowrank, nowhr->getElem(),  nowhr->getLeft(), std::make_shared<Heap>(Heap::empty()));
                    now = now->getRight();
                    nowhr = nowhr->getRight();
                } else {
                    *now = Heap(nowrank, nowhr->getElem(), std::make_shared<Heap>(Heap::empty()), nowhr->getLeft());
                    now = now->getLeft();
                    nowhr = nowhr->getRight();
                }
            }
        }
        return *head.get();
    }
    Heap insert(Elem x) {
        static auto visit_insert = overloaded {
            [] (Empty e, Elem x) {
                return Heap(x);
            },
            [] (SubHeap h, Elem x) {
                if (h.elem < x) {
                    auto rheap = std::make_shared<Heap>(h.right->insert(x));
                    if (h.left->rank() >= rheap->rank())
                        return Heap(rheap->rank() + h.left->rank() + 1, h.elem, h.left, rheap);
                    else
                        return Heap(rheap->rank() + h.left->rank() + 1, h.elem, rheap, h.left);
                }
                else
                    return Heap(1, x, h, Heap::empty());
            },
        };
        return std::visit(std::bind(visit_insert, _1, x), m_heap);
    }
    Elem findMin() {
        static auto visit_findMin = overloaded {
            [] (Empty e) {
                throw std::runtime_error("No element");
                return 0;
            },
            [] (SubHeap h) {
                return h.elem;
            },
        };
        return std::visit(visit_findMin, m_heap);
    }
    Heap deleteMin() {
        static auto visit_deleteMin = overloaded {
            [] (Empty e) {
                throw std::runtime_error("No element");
                return Heap();
            },
            [] (SubHeap h) {
                return h.left->merge(*h.right);
            },
        };
        return std::visit(visit_deleteMin, m_heap);
    }
    int rank() {
        static auto visit_rank = overloaded{
            [] (Empty e) {
                return 0;
            },
            [] (SubHeap x) {
                return x.rank;
            },
        };
        return std::visit(visit_rank, m_heap);
    }
    void print() const {
        static auto visit_print = overloaded {
            [](Empty e) { },
            [](SubHeap t) {
                std::cout << t.elem << "(";
                t.left->print();
                std::cout << " ) : ( ";
                t.right->print();
                std::cout << ")";
            }
        };
        std::visit(visit_print, m_heap);
    }
private:
    bool isEmpty() {
        return std::holds_alternative<Empty>(m_heap);
    }
    Elem getElem() {
        return std::get<SubHeap>(m_heap).elem;
    }
    Elem getRank() {
        return std::get<SubHeap>(m_heap).rank;
    }
    std::shared_ptr<Heap> getLeft() {
        return std::get<SubHeap>(m_heap).left;
    }
    std::shared_ptr<Heap> getRight() {
        return std::get<SubHeap>(m_heap).right;
    }
    static Heap makeT(Elem x, std::shared_ptr<Heap> a, std::shared_ptr<Heap> b) {
        if (a->rank() >= b->rank())
            return Heap(a->rank() + b->rank() + 1, x, a, b);
        else
            return Heap(a->rank() + b->rank() + 1, x, b, a);
    }
    std::variant<Empty, SubHeap> m_heap;
};


int main() {
    Heap<int> hp = Heap<int>::Heapify({1,2,3,4});
    auto hp2 = hp.insert(3).insert(-10).insert(100).insert(1310).insert(0).insert(0);
    auto hp3 = hp.insert(4).insert(6);
    hp2.print(); std::cout << std::endl;
    hp3.print(); std::cout << std::endl;
    auto hpi = merge_iterative(hp2,hp3);
    hpi.print(); std::cout << std::endl;
}
