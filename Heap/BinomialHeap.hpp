#ifndef BINOMIALHEAP
#define BINOMIALHEAP
#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <stack>
#include "../utils/utils.hpp"
#include "../List/List.hpp"

using namespace std::placeholders;

template <Ordered Elem>
class BinomialHeap {
private:
    class Tree {
    public:
        Tree() = default;
        Tree(int rk, Elem e, ImList<Tree> lis) : rank(rk), elem(e), children(lis) { }
        int rank;
        Elem elem;
        ImList<Tree> children;
        friend Tree link(const Tree t1, const Tree t2) {
            if (t1.elem <= t2.elem)
                return Tree(t1.rank + 1, t1.elem, cons(t2, t1.children));
            else
                return Tree(t1.rank + 1, t2.elem, cons(t1, t2.children));
        }
        int getrank() const {
            return rank;
        }
        int getelem() const {
            return elem;
        }
    };
    ImList<Tree> m_heap;
    bool isEmpty() const {
        return m_heap.isEmpty();
    }
    Tree head() const {
        return m_heap.head();
    }
    BinomialHeap tail() const {
        return m_heap.tail();
    }
    friend BinomialHeap cons(Tree t, BinomialHeap h) {
        return cons(t, h.m_heap);
    }
public:
    BinomialHeap() = default;
    BinomialHeap(ImList<Tree> it) : m_heap(it) { }
    BinomialHeap insTree(Tree t) const { // always t.getrank() <= t_min.getrank()
        if (isEmpty())
            return cons(t, *this);

        auto t_min = head();
        if (t.getrank() < t_min.getrank()) 
            return cons(t, *this);
        else
            return tail().insTree(link(t, t_min));
    }
    BinomialHeap insert(Elem x) {
        return insTree({0, x, {}});
    }
    friend BinomialHeap merge(BinomialHeap h1, BinomialHeap h2) {
        if (h1.isEmpty())
            return h2;
        if (h2.isEmpty())
            return h1;

        auto head1 = h1.head(), head2 = h2.head();
        auto tail1 = h1.tail(), tail2 = h2.tail();
        if (head1.getrank() < head2.getrank())
            return cons(head1, merge(tail1, h2));
        else if (head2.getrank() < head1.getrank()) 
            return cons(head2, merge(h1, tail2));
        else
            return merge(tail1, tail2).insTree(link(head1, head2));
    }
    std::pair<Tree, BinomialHeap> removeMinTree() {
        if (tail().isEmpty())
            return {head(), {}};
        else {
            auto [tp, tps] = tail().removeMinTree();
            if (head().elem < tp.elem)
                return {head(), tail()};
            else
                return {tp, cons(head(), tps)};
        }
    }
    Elem findMin() {
        auto [t, tmp] = removeMinTree();
        return t.elem;
    }
    Elem findMin_direct() {
        if (isEmpty())
            throw std::runtime_error("no element");
        Elem e = head().elem;
        for (auto t : m_heap) {
            if (t.elem < e)
                e = t.elem;
        }
        return e;
    }
    BinomialHeap deleteMin() {
        auto [t, ts] = removeMinTree();
        return merge(t.children.rev(), ts);
    }
    void print() {
        for (auto t: m_heap) {
            print_tree(t);
        }
    }
    friend void print_tree(Tree t) {
        std::cout << t.elem << std::endl;
        for (auto ch: t.children) {
            print_tree(ch);
        }
    }
};

#endif
