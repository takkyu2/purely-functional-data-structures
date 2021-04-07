#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <stack>
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"
#include "../List/List.hpp"

template <Ordered Elem>
class Heap {
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
    std::shared_ptr<FunctionPtr<ImList<Tree>()>> m_heap;
    bool isEmpty() const {
        return force().isEmpty();
    }
    Tree head() const {
        return force().head();
    }
    Heap tail() const {
        return make_thunk([this_heap=*this](){ return this_heap.tail();});
    }
    friend Heap cons(Tree t, Heap h) {
        return make_thunk([t,h]() { return cons(t, h.force());});
    }
    ImList<Tree> force() const {
        return (*m_heap)();
    }
    template <typename F>
    static Heap make_thunk(F&& lam) {
        return Heap(make_lazy_val(std::forward<F>(lam)));
    }
public:
    Heap() {
        m_heap = std::make_shared<FunctionPtr<ImList<Tree>()>>(
                make_lazy_val([](){return ImList<Tree>();})
                );
    }
    Heap(FunctionPtr<ImList<Tree>()> it) 
        : m_heap(std::make_shared<FunctionPtr<ImList<Tree>()>>(it)) { }
    friend ImList<Tree> insTree(Tree t, ImList<Tree> ts) { // always t.getrank() <= t_min.getrank()
        if (ts.isEmpty())
            return cons(t, ts);

        auto t_min = ts.head();
        if (t.getrank() < t_min.getrank()) 
            return cons(t, ts);
        else
            return insTree(link(t, t_min), ts.tail());
    }
    Heap insert(Elem x) {
        return make_thunk([this_heap=*this, x](){return insTree({0, x, {}}, this_heap.force());});
    }
    friend Heap merge(Heap h1, Heap h2) { // like adding two numbers
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
            return make_thunk([head1,head2,tail1,tail2](){return insTree(link(head1, head2), merge(tail1, tail2).force());});
    }
    std::pair<Tree, Heap> removeMinTree() {
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
        for (auto t : force()) {
            if (t.elem < e)
                e = t.elem;
        }
        return e;
    }
    Heap deleteMin() {
        auto [t, ts] = removeMinTree();
        return merge(make_thunk([t=t](){return t.children.rev();}), ts);
    }
    void print() {
        for (auto t: force()) {
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


int main() {
    Heap<int> hp;
    hp = hp.insert(3).insert(4).insert(8);
    auto hp2 = hp.insert(10).insert(-100);
    hp.print();
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    hp2.print();
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    hp2 = hp2.deleteMin();
    hp2.print();
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    auto hp3 = merge(hp, hp2);
    hp3.print();
}
