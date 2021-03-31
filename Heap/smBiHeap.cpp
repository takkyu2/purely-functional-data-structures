#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <stack>
#include "../utils/utils.hpp"
#include "../List_im/List_im.hpp"

using namespace std::placeholders;

template <Ordered Elem>
class Heap {
private:
    class Tree {
    public:
        Tree() = default;
        Tree(Elem e, ImList<Tree> lis) : elem(e), children(lis) { }
        Elem elem;
        ImList<Tree> children;
        friend Tree link(const Tree t1, const Tree t2) {
            if (t1.elem <= t2.elem)
                return Tree(t1.elem, cons(t2, t1.children));
            else
                return Tree(t2.elem, cons(t1, t2.children));
        }
        int getelem() const {
            return elem;
        }
    };
    ImList<std::pair<int, Tree>> m_heap;
    std::pair<int, Tree> head() const {
        return m_heap.head();
    }
    Heap tail() const {
        return m_heap.tail();
    }
    friend Heap cons(std::pair<int, Tree> t, Heap h) {
        return cons(t, h.m_heap);
    }
public:
    Heap() = default;
    Heap(ImList<std::pair<int,Tree>> it) : m_heap(it) { }
    bool isEmpty() const {
        return m_heap.isEmpty();
    }
    Heap insTree(Tree t, int rk) const { // always t.getrank() <= t_min.getrank()
        if (isEmpty())
            return cons({rk, t}, *this);

        auto [rk_min, t_min] = head();
        if (rk < rk_min) 
            return cons({rk, t}, *this);
        else
            return tail().insTree(link(t, t_min), rk+1);
    }
    Heap insert(Elem x) {
        return insTree({x, {}}, 0);
    }
    friend Heap merge(Heap h1, Heap h2) {
        if (h1.isEmpty())
            return h2;
        if (h2.isEmpty())
            return h1;

        auto [h1rk, head1] = h1.head();
        auto [h2rk, head2] = h2.head();
        auto tail1 = h1.tail(), tail2 = h2.tail();
        if (h1rk < h2rk)
            return cons(h1.head(), merge(tail1, h2));
        else if (h2rk < h1rk) 
            return cons(h2.head(), merge(h1, tail2));
        else
            return merge(tail1, tail2).insTree(link(head1, head2), h1rk+1);
    }
    std::pair<Tree, Heap> removeMinTree() {
        if (tail().isEmpty())
            return {head().second, {}};
        else {
            auto [tp, tps] = tail().removeMinTree();
            if (head().second.elem < tp.elem)
                return {head().second, tail()};
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
        Elem e = head().second.elem;
        for (auto t : m_heap) {
            if (t.second.elem < e)
                e = t.second.elem;
        }
        return e;
    }
    Heap deleteMin() {
        auto [t, ts] = removeMinTree();
        ImList<std::pair<int, Tree>> tr;
        int cnt = 0;
        for (auto t: t.children) {
            tr = cons({cnt++, t}, tr);
        }
        return merge(tr, ts);
    }
    void print() {
        for (auto [rk, t]: m_heap) {
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

template <Ordered Elem>
class ExplicitMin : Heap<Elem> {
public:
    ExplicitMin() = default;
    ExplicitMin(std::pair<int, Heap<Elem>> st) : m_heap(st) { }
    Elem findMin() {
        static auto visit_findmin = overloaded {
            [](Empty e) {
                throw std::runtime_error("no element");
                return -1;
            },
            [](std::pair<int, Heap<Elem>> h) {
                return h.first;
            }
        };
        return std::visit(visit_findmin, m_heap);
    }
    ExplicitMin insert(Elem x) {
        static auto visit_insert = overloaded {
            [](Empty e, Elem x) {
                return ExplicitMin(std::make_pair(x, Heap<Elem>{}));
            },
            [](std::pair<int, Heap<Elem>> h, Elem x) {
                if (x < h.first)
                    return ExplicitMin(std::make_pair(x, h.second.insert(h.first)));
                else 
                    return ExplicitMin(std::make_pair(h.first, h.second.insert(x)));
            }
        };
        return std::visit(std::bind(visit_insert, _1, x), m_heap);
    }
    friend ExplicitMin merge(ExplicitMin h1, ExplicitMin h2) {
        static auto visit_merge = overloaded {
            [](auto x, Empty e) {
                return x;
            },
            [](Empty e, auto x) {
                return x;
            },
            [](std::pair<int, Heap<Elem>> h1, std::pair<int, Heap<Elem>> h2) {
                if (h1.first < h2.first) {
                    return std::make_pair(h1.first, merge(h1.second,h2.second).insert(h2.first));
                } else {
                    return std::make_pair(h2.first, merge(h1.second,h2.second).insert(h1.first));
                }
            }
        };
        return std::visit(visit_merge, h1, h2);
    }
    ExplicitMin deleteMin() {
        static auto visit_deletemin = overloaded {
            [](Empty e) {
                return ExplicitMin {};
            },
            [](std::pair<int, Heap<Elem>> h) {
                if (h.second.isEmpty()) {
                    return ExplicitMin {};
                } else {
                    return ExplicitMin(std::make_pair(h.second.findMin(), h.second.deleteMin()));
                }
            }
        };
        return std::visit(visit_deletemin, m_heap);
    }
    void print() {
        static auto visit_print = overloaded {
            [](Empty e) {
                return;
            },
            [](std::pair<int, Heap<Elem>> h) {
                std::cout << "Min: " << h.first << std::endl;
                std::cout << "Rest: " << std::endl;
                h.second.print();
            }
        };
        return std::visit(visit_print, m_heap);

    }
private:
    std::variant<Empty, std::pair<int,Heap<Elem>>> m_heap;
};

int main() {
    ExplicitMin<int> hp;
    hp = hp.insert(3).insert(4).insert(8);
    auto hp2 = hp.insert(10).insert(-100);
    /* hp.print(); */
    std::cout << hp2.findMin() << std::endl;
    /* std::cout << hp2.findMin_direct() << std::endl; */
    hp2 = hp2.deleteMin();
    hp2.print();
    /* hp2.print(); std::cout << std::endl; */
    /* hp3.print(); std::cout << std::endl; */
    /* std::cout << hp2.findMin() << std::endl; */
    /* hp2 = hp2.deleteMin(); */
    /* hp2 = hp2.deleteMin(); */
    /* hp3 = hp3.merge(hp2); */
    /* Heap<int> hp4 = Heap<int>::Heapify({1,2,3,4}); */
    /* Heap<int> hp5 = Heap<int>::Heapify({5,6}); */
    /* auto hpi = merge_iterative(hp2,hp3); */
    /* hpi.print(); std::cout << std::endl; */
}
