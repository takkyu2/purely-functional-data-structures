#ifndef LEFTIESTHEAP
#define LEFTIESTHEAP
#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <stack>
#include "../utils/utils.hpp"

using namespace std::placeholders;

template <Ordered Elem>
class LeftistHeap {
private:
    struct SubLeftistHeap {
        SubLeftistHeap(Elem x) : rank(1), elem(x) { }
        SubLeftistHeap(int rk, Elem x, std::shared_ptr<LeftistHeap> l, std::shared_ptr<LeftistHeap> r) 
            : rank(rk), elem(x), left(l), right(r) { }
        int rank;
        Elem elem;
        std::shared_ptr<LeftistHeap> left, right;
    };
public:
    LeftistHeap() = default;
    LeftistHeap(Empty e) { }
    LeftistHeap(SubLeftistHeap t) : m_heap(t) { }
    LeftistHeap(const LeftistHeap& bst) = default;
    static LeftistHeap Heapify(std::initializer_list<Elem> il) {
        if (il.size() == 0) return {};
        std::stack<LeftistHeap> now_st, next_st;
        for (Elem e: il) now_st.push(e);
        while (now_st.size() > 1) {
            while (now_st.size() >= 2) {
                LeftistHeap h1 = now_st.top(); now_st.pop();
                LeftistHeap h2 = now_st.top(); now_st.pop();
                next_st.push(h1.merge(h2));
            }
            if (now_st.size() == 1) {
                LeftistHeap h1 = now_st.top(); now_st.pop();
                next_st.push(h1);
            }
            swap(now_st, next_st);
        }
        return now_st.top();
    }
    LeftistHeap(Elem x)
        : m_heap(SubLeftistHeap(
                    1,
                    x,
                    std::make_shared<LeftistHeap>(LeftistHeap::empty()), 
                    std::make_shared<LeftistHeap>(LeftistHeap::empty())
                )
                ) { }
    LeftistHeap(int rank, Elem x, std::shared_ptr<LeftistHeap> l, std::shared_ptr<LeftistHeap> r)
        : m_heap(SubLeftistHeap(rank, x, l, r)) { }
    LeftistHeap(int rank, Elem x, LeftistHeap l, LeftistHeap r)
        : m_heap(SubLeftistHeap(rank, x, std::make_shared<LeftistHeap>(l), std::make_shared<LeftistHeap>(r))) { }
    static LeftistHeap empty() { return {};}
    LeftistHeap merge(LeftistHeap h) const {
        static auto visit_merge = overloaded{
            [] (Empty e, Empty x) {
                return LeftistHeap(x);
            },
            [] (Empty e, auto x) {
                return LeftistHeap(x);
            },
            [] (auto x, Empty e) {
                return LeftistHeap(x);
            },
            [] (SubLeftistHeap h1, SubLeftistHeap h2) {
                if (h1.elem < h2.elem)
                    return makeT(h1.elem, h1.left, std::make_shared<LeftistHeap>(h1.right->merge(h2)));
                else
                    return makeT(h2.elem, h2.left, std::make_shared<LeftistHeap>(h2.right->merge(h1)));
            },
        };
        return std::visit(visit_merge, m_heap, h.m_heap);
    }
    LeftistHeap insert(Elem x) {
        static auto visit_insert = overloaded {
            [] (Empty e, Elem x) {
                return LeftistHeap(x);
            },
            [] (SubLeftistHeap h, Elem x) {
                if (h.elem < x) {
                    auto rheap = std::make_shared<LeftistHeap>(h.right->insert(x));
                    if (h.left->rank() >= rheap->rank())
                        return LeftistHeap(rheap->rank() + 1, h.elem, h.left, rheap);
                    else
                        return LeftistHeap(h.left->rank() + 1, h.elem, rheap, h.left);
                }
                else
                    return LeftistHeap(1, x, h, LeftistHeap::empty());
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
            [] (SubLeftistHeap h) {
                return h.elem;
            },
        };
        return std::visit(visit_findMin, m_heap);
    }
    LeftistHeap deleteMin() {
        static auto visit_deleteMin = overloaded {
            [] (Empty e) {
                throw std::runtime_error("No element");
                return LeftistHeap();
            },
            [] (SubLeftistHeap h) {
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
            [] (SubLeftistHeap x) {
                return x.rank;
            },
        };
        return std::visit(visit_rank, m_heap);
    }
    void print() const {
        static auto visit_print = overloaded {
            [](Empty e) { },
            [](SubLeftistHeap t) {
                std::cout << t.elem << "(";
                t.left->print();
                std::cout << " ) : ( ";
                t.right->print();
                std::cout << ")";
            }
        };
        std::visit(visit_print, m_heap);
    }
    Elem getElem(Elem x) const {
        if (std::holds_alternative<Empty>(m_heap))
            throw std::runtime_error("element not found, empty");
        else
            return getElem_helper(x, std::get<SubLeftistHeap>(m_heap).elem);
    }
private:
    static LeftistHeap makeT(Elem x, std::shared_ptr<LeftistHeap> a, std::shared_ptr<LeftistHeap> b) {
        if (a->rank() >= b->rank())
            return LeftistHeap(b->rank() + 1, x, a, b);
        else
            return LeftistHeap(a->rank() + 1, x, b, a);
    }
    std::variant<Empty, SubLeftistHeap> m_heap;
};

#endif
