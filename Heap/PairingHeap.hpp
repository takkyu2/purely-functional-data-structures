#ifndef PAIRINGHEAP
#define PAIRINGHEAP
#include <variant>
#include "../utils/utils.hpp"
#include "../List/List.hpp"

template <Ordered Elem>
class PairingHeap {
public:
    PairingHeap() = default;
    PairingHeap(Elem e) : m_heap(Tree(e)) { }
    PairingHeap(Elem e, std::shared_ptr<PairingHeap> left, std::shared_ptr<PairingHeap> right)
        : m_heap(Tree(e, left, right)) { }
    PairingHeap(Elem e, std::shared_ptr<PairingHeap> left)
        : m_heap(Tree(e, left)) { }
    bool isEmpty() {
        return std::holds_alternative<Empty>(m_heap);
    }
    Elem findMin() {
        if (isEmpty()) throw std::runtime_error("error in findMin, empty");
        return std::get<Tree>(m_heap).getelem();
    }
    friend PairingHeap merge(PairingHeap h1, PairingHeap h2) {
        if (h1.isEmpty()) return h2;
        if (h2.isEmpty()) return h1;
        auto t1 = h1.get();
        auto t2 = h2.get();
        if (t1.getelem() < t2.getelem()) {
            auto newleft = std::make_shared<PairingHeap>(PairingHeap(t2.elem, t2.left, t1.left));
            return PairingHeap(t1.getelem(), newleft);
        } else {
            auto newleft = std::make_shared<PairingHeap>(PairingHeap(t1.elem, t1.left, t2.left));
            return PairingHeap(t2.getelem(), newleft);
        }
    }
    PairingHeap insert(Elem e) {
        return merge(PairingHeap(e), *this);
    }
    PairingHeap mergePairs() {
        if (isEmpty()) return PairingHeap();
        auto t = get();
        auto left = t.left, right = t.right;
        if (right->isEmpty()) return *this;
        auto t_right = right->get();
        auto r_right = t_right.right;
        return merge(merge(PairingHeap(t.getelem(), left), PairingHeap(t_right.getelem(), t_right.left)), r_right->mergePairs());
    }
    PairingHeap deleteMin() {
        if (isEmpty()) return PairingHeap();
        auto t = get();
        return t.left->mergePairs();
    }
private:
    struct Tree {
        Tree() = default;
        Tree(Elem e) : elem(e) { }
        Tree(Elem e, std::shared_ptr<PairingHeap> left, std::shared_ptr<PairingHeap> right) 
            : elem(e), left(left), right(right) { }
        Tree(Elem e, std::shared_ptr<PairingHeap> left) 
            : elem(e), left(left) { }
        Elem getelem() {
            return elem;
        }
        Elem elem;
        std::shared_ptr<PairingHeap> left, right;
    };
    Tree get() {
        auto t = std::get<Tree>(m_heap);
        return t;
    }
    std::variant<Empty, Tree> m_heap;
};

template <Ordered Elem>
class Heap {
public:
    template <Ordered T>
    friend PairingHeap<T> toBinary(Heap<T>);
    template <Ordered T>
    friend PairingHeap<T> toBinary_helper(Heap<T>, ImList<std::shared_ptr<Heap<T>>>);
    Heap() = default;
    Heap(Elem e) : m_heap(Tree(e)) { }
    Heap(Elem e, ImList<std::shared_ptr<Heap>> children) : m_heap(Tree(e, children)) { }
    bool isEmpty() {
        return std::holds_alternative<Empty>(m_heap);
    }
    Elem findMin() {
        if (isEmpty()) throw std::runtime_error("error in findMin, empty");
        return std::get<Tree>(m_heap).getelem();
    }
    friend Heap merge(Heap h1, Heap h2) {
        if (h1.isEmpty()) return h2;
        if (h2.isEmpty()) return h1;
        auto [x, h1c] = h1.get();
        auto [y, h2c] = h2.get();
        if (x < y)
            return Heap(x, cons(std::make_shared<Heap>(h2), h1c));
        else
            return Heap(y, cons(std::make_shared<Heap>(h1), h2c));
    }
    Heap insert(Elem e) {
        return merge(Heap(e), *this);
    }
    friend Heap mergePairs(ImList<std::shared_ptr<Heap>> hc) {
        if (hc.isEmpty()) return Heap();
        if (hc.tail().isEmpty()) return *hc.head();
        auto h1 = hc.head(), h2 = hc.tail().head(), hs = hc.tail().tail();
        return merge(merge(*h1, *h2), mergePairs(hs));
    }
    Heap deleteMin() {
        if (isEmpty()) return Heap();
        auto [x, hc] = get();
        return mergePairs(hc);
    }
private:
    struct Tree {
        Tree() = default;
        Tree(Elem e) : elem(e) { }
        Tree(Elem e, ImList<std::shared_ptr<Heap>> lis) : elem(e), children(lis) { }
        Elem getelem() {
            return elem;
        }
        Elem elem;
        ImList<std::shared_ptr<Heap>> children;
    };
    std::pair<Elem, ImList<std::shared_ptr<Heap>>> get() {
        auto t = std::get<Tree>(m_heap);
        return {t.elem, t.children};
    }
    std::variant<Empty, Tree> m_heap;
};

template <Ordered Elem>
PairingHeap<Elem> toBinary(Heap<Elem> h) {
    return toBinary_helper(h, ImList<std::shared_ptr<Heap<Elem>>>());
}

template <Ordered Elem>
PairingHeap<Elem> toBinary_helper(Heap<Elem> h, ImList<std::shared_ptr<Heap<Elem>>> sib) {
    if (h.isEmpty()) return PairingHeap<Elem>();
    auto [x, hc] = h.get();
    if (hc.isEmpty()) {
        if (sib.isEmpty())
            return PairingHeap<Elem>(x);
        else
            return PairingHeap<Elem>(x, 
                    std::make_shared<PairingHeap<Elem>>(PairingHeap<Elem>()), 
                    std::make_shared<PairingHeap<Elem>>(toBinary_helper(*sib.head(), sib.tail()))
                    );
    } else {
        if (sib.isEmpty())
            return PairingHeap<Elem>(x
                    ,std::make_shared<PairingHeap<Elem>>(toBinary_helper(*hc.head(), hc.tail()))
                    ,std::make_shared<PairingHeap<Elem>>(PairingHeap<Elem>())
                    );
        else
            return PairingHeap<Elem>(x
                    ,std::make_shared<PairingHeap<Elem>>(toBinary_helper(*hc.head(), hc.tail()))
                    ,std::make_shared<PairingHeap<Elem>>(toBinary_helper(*sib.head(), sib.tail()))
                    );
    }
}


#endif
