#ifndef WEIGHTBIASEDLEFTIESTHEAP
#define WEIGHTBIASEDLEFTIESTHEAP
#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <stack>
#include "../utils/utils.hpp"

using namespace std::placeholders;

template <Ordered Elem>
class WeightBiasedLeftistHeap {
private:
    struct SubWeightBiasedLeftistHeap {
        SubWeightBiasedLeftistHeap(Elem x) : rank(1), elem(x) { }
        SubWeightBiasedLeftistHeap(int rk, Elem x, std::shared_ptr<WeightBiasedLeftistHeap> l, std::shared_ptr<WeightBiasedLeftistHeap> r) 
            : rank(rk), elem(x), left(l), right(r) { }
        int rank;
        Elem elem;
        std::shared_ptr<WeightBiasedLeftistHeap> left, right;
    };
public:
    WeightBiasedLeftistHeap() = default;
    WeightBiasedLeftistHeap(Empty e) { }
    WeightBiasedLeftistHeap(SubWeightBiasedLeftistHeap t) : m_heap(t) { }
    WeightBiasedLeftistHeap(const WeightBiasedLeftistHeap& bst) = default;
    static WeightBiasedLeftistHeap Heapify(std::initializer_list<Elem> il) {
        if (il.size() == 0) return {};
        std::stack<WeightBiasedLeftistHeap> now_st, next_st;
        for (Elem e: il) now_st.push(e);
        while (now_st.size() > 1) {
            while (now_st.size() >= 2) {
                WeightBiasedLeftistHeap h1 = now_st.top(); now_st.pop();
                WeightBiasedLeftistHeap h2 = now_st.top(); now_st.pop();
                next_st.push(h1.merge(h2));
            }
            if (now_st.size() == 1) {
                WeightBiasedLeftistHeap h1 = now_st.top(); now_st.pop();
                next_st.push(h1);
            }
            swap(now_st, next_st);
        }
        return now_st.top();
    }
    WeightBiasedLeftistHeap(Elem x)
        : m_heap(SubWeightBiasedLeftistHeap(
                    1,
                    x,
                    std::make_shared<WeightBiasedLeftistHeap>(WeightBiasedLeftistHeap::empty()), 
                    std::make_shared<WeightBiasedLeftistHeap>(WeightBiasedLeftistHeap::empty())
                )
                ) { }
    WeightBiasedLeftistHeap(int rank, Elem x, std::shared_ptr<WeightBiasedLeftistHeap> l, std::shared_ptr<WeightBiasedLeftistHeap> r)
        : m_heap(SubWeightBiasedLeftistHeap(rank, x, l, r)) { }
    WeightBiasedLeftistHeap(int rank, Elem x, WeightBiasedLeftistHeap l, WeightBiasedLeftistHeap r)
        : m_heap(SubWeightBiasedLeftistHeap(rank, x, std::make_shared<WeightBiasedLeftistHeap>(l), std::make_shared<WeightBiasedLeftistHeap>(r))) { }
    static WeightBiasedLeftistHeap empty() { return {};}
    WeightBiasedLeftistHeap merge(WeightBiasedLeftistHeap h) const {
        static auto visit_merge = overloaded{
            [] (Empty e, Empty x) {
                return WeightBiasedLeftistHeap(x);
            },
            [] (Empty e, auto x) {
                return WeightBiasedLeftistHeap(x);
            },
            [] (auto x, Empty e) {
                return WeightBiasedLeftistHeap(x);
            },
            [] (SubWeightBiasedLeftistHeap h1, SubWeightBiasedLeftistHeap h2) {
                if (h1.elem < h2.elem)
                    return makeT(h1.elem, h1.left, std::make_shared<WeightBiasedLeftistHeap>(h1.right->merge(h2)));
                else
                    return makeT(h2.elem, h2.left, std::make_shared<WeightBiasedLeftistHeap>(h2.right->merge(h1)));
            },
        };
        return std::visit(visit_merge, m_heap, h.m_heap);
    }
    friend WeightBiasedLeftistHeap merge_iterative(WeightBiasedLeftistHeap hl, WeightBiasedLeftistHeap hr) {
        auto head = std::make_shared<WeightBiasedLeftistHeap>(WeightBiasedLeftistHeap::empty());
        auto now = head;
        auto nowhl = std::make_shared<WeightBiasedLeftistHeap>(hl), nowhr = std::make_shared<WeightBiasedLeftistHeap>(hr);
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
                    *now = WeightBiasedLeftistHeap(nowrank, nowhl->getElem(),  nowhl->getLeft(), std::make_shared<WeightBiasedLeftistHeap>(WeightBiasedLeftistHeap::empty()));
                    now = now->getRight();
                    nowhl = nowhl->getRight();
                } else {
                    *now = WeightBiasedLeftistHeap(nowrank, nowhl->getElem(), std::make_shared<WeightBiasedLeftistHeap>(WeightBiasedLeftistHeap::empty()), nowhl->getLeft());
                    now = now->getLeft();
                    nowhl = nowhl->getRight();
                }
            } else {
                if (nowhr->getLeft()->rank() >= nowhr->getRight()->rank() + nowhl->rank()) {
                    *now = WeightBiasedLeftistHeap(nowrank, nowhr->getElem(),  nowhr->getLeft(), std::make_shared<WeightBiasedLeftistHeap>(WeightBiasedLeftistHeap::empty()));
                    now = now->getRight();
                    nowhr = nowhr->getRight();
                } else {
                    *now = WeightBiasedLeftistHeap(nowrank, nowhr->getElem(), std::make_shared<WeightBiasedLeftistHeap>(WeightBiasedLeftistHeap::empty()), nowhr->getLeft());
                    now = now->getLeft();
                    nowhr = nowhr->getRight();
                }
            }
        }
        return *head.get();
    }
    WeightBiasedLeftistHeap insert(Elem x) {
        static auto visit_insert = overloaded {
            [] (Empty e, Elem x) {
                return WeightBiasedLeftistHeap(x);
            },
            [] (SubWeightBiasedLeftistHeap h, Elem x) {
                if (h.elem < x) {
                    auto rheap = std::make_shared<WeightBiasedLeftistHeap>(h.right->insert(x));
                    if (h.left->rank() >= rheap->rank())
                        return WeightBiasedLeftistHeap(rheap->rank() + h.left->rank() + 1, h.elem, h.left, rheap);
                    else
                        return WeightBiasedLeftistHeap(rheap->rank() + h.left->rank() + 1, h.elem, rheap, h.left);
                }
                else
                    return WeightBiasedLeftistHeap(1, x, h, WeightBiasedLeftistHeap::empty());
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
            [] (SubWeightBiasedLeftistHeap h) {
                return h.elem;
            },
        };
        return std::visit(visit_findMin, m_heap);
    }
    WeightBiasedLeftistHeap deleteMin() {
        static auto visit_deleteMin = overloaded {
            [] (Empty e) {
                throw std::runtime_error("No element");
                return WeightBiasedLeftistHeap();
            },
            [] (SubWeightBiasedLeftistHeap h) {
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
            [] (SubWeightBiasedLeftistHeap x) {
                return x.rank;
            },
        };
        return std::visit(visit_rank, m_heap);
    }
    void print() const {
        static auto visit_print = overloaded {
            [](Empty e) { },
            [](SubWeightBiasedLeftistHeap t) {
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
        return std::get<SubWeightBiasedLeftistHeap>(m_heap).elem;
    }
    Elem getRank() {
        return std::get<SubWeightBiasedLeftistHeap>(m_heap).rank;
    }
    std::shared_ptr<WeightBiasedLeftistHeap> getLeft() {
        return std::get<SubWeightBiasedLeftistHeap>(m_heap).left;
    }
    std::shared_ptr<WeightBiasedLeftistHeap> getRight() {
        return std::get<SubWeightBiasedLeftistHeap>(m_heap).right;
    }
    static WeightBiasedLeftistHeap makeT(Elem x, std::shared_ptr<WeightBiasedLeftistHeap> a, std::shared_ptr<WeightBiasedLeftistHeap> b) {
        if (a->rank() >= b->rank())
            return WeightBiasedLeftistHeap(a->rank() + b->rank() + 1, x, a, b);
        else
            return WeightBiasedLeftistHeap(a->rank() + b->rank() + 1, x, b, a);
    }
    std::variant<Empty, SubWeightBiasedLeftistHeap> m_heap;
};


#endif
