#include <variant>
#include "../utils/utils.hpp"
#include "../List/List.hpp"

template <Ordered Elem>
class BinaryHeap {
public:
    BinaryHeap() = default;
    BinaryHeap(Elem e) : m_heap(Tree(e)) { }
    BinaryHeap(Elem e, std::shared_ptr<BinaryHeap> left, std::shared_ptr<BinaryHeap> right)
        : m_heap(Tree(e, left, right)) { }
    BinaryHeap(Elem e, std::shared_ptr<BinaryHeap> left)
        : m_heap(Tree(e, left)) { }
    bool isEmpty() {
        return std::holds_alternative<Empty>(m_heap);
    }
    Elem findMin() {
        if (isEmpty()) throw std::runtime_error("error in findMin, empty");
        return std::get<Tree>(m_heap).getelem();
    }
    friend BinaryHeap merge(BinaryHeap h1, BinaryHeap h2) {
        if (h1.isEmpty()) return h2;
        if (h2.isEmpty()) return h1;
        auto t1 = h1.get();
        auto t2 = h2.get();
        if (t1.getelem() < t2.getelem()) {
            auto newleft = std::make_shared<BinaryHeap>(BinaryHeap(t2.elem, t2.left, t1.left));
            return BinaryHeap(t1.getelem(), newleft);
        } else {
            auto newleft = std::make_shared<BinaryHeap>(BinaryHeap(t1.elem, t1.left, t2.left));
            return BinaryHeap(t2.getelem(), newleft);
        }
    }
    BinaryHeap insert(Elem e) {
        return merge(BinaryHeap(e), *this);
    }
    BinaryHeap mergePairs() {
        if (isEmpty()) return BinaryHeap();
        auto t = get();
        auto left = t.left, right = t.right;
        if (right->isEmpty()) return *this;
        auto t_right = right->get();
        auto r_right = t_right.right;
        return merge(merge(BinaryHeap(t.getelem(), left), BinaryHeap(t_right.getelem(), t_right.left)), r_right->mergePairs());
    }
    BinaryHeap deleteMin() {
        if (isEmpty()) return BinaryHeap();
        auto t = get();
        return t.left->mergePairs();
    }
private:
    struct Tree {
        Tree() = default;
        Tree(Elem e) : elem(e) { }
        Tree(Elem e, std::shared_ptr<BinaryHeap> left, std::shared_ptr<BinaryHeap> right) 
            : elem(e), left(left), right(right) { }
        Tree(Elem e, std::shared_ptr<BinaryHeap> left) 
            : elem(e), left(left) { }
        Elem getelem() {
            return elem;
        }
        Elem elem;
        std::shared_ptr<BinaryHeap> left, right;
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
    friend BinaryHeap<T> toBinary(Heap<T>);
    template <Ordered T>
    friend BinaryHeap<T> toBinary_helper(Heap<T>, ImList<std::shared_ptr<Heap<T>>>);
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
BinaryHeap<Elem> toBinary(Heap<Elem> h) {
    return toBinary_helper(h, ImList<std::shared_ptr<Heap<Elem>>>());
}

template <Ordered Elem>
BinaryHeap<Elem> toBinary_helper(Heap<Elem> h, ImList<std::shared_ptr<Heap<Elem>>> sib) {
    if (h.isEmpty()) return BinaryHeap<Elem>();
    auto [x, hc] = h.get();
    if (hc.isEmpty()) {
        if (sib.isEmpty())
            return BinaryHeap<Elem>(x);
        else
            return BinaryHeap<Elem>(x, 
                    std::make_shared<BinaryHeap<Elem>>(BinaryHeap<Elem>()), 
                    std::make_shared<BinaryHeap<Elem>>(toBinary_helper(*sib.head(), sib.tail()))
                    );
    } else {
        if (sib.isEmpty())
            return BinaryHeap<Elem>(x
                    ,std::make_shared<BinaryHeap<Elem>>(toBinary_helper(*hc.head(), hc.tail()))
                    ,std::make_shared<BinaryHeap<Elem>>(BinaryHeap<Elem>())
                    );
        else
            return BinaryHeap<Elem>(x
                    ,std::make_shared<BinaryHeap<Elem>>(toBinary_helper(*hc.head(), hc.tail()))
                    ,std::make_shared<BinaryHeap<Elem>>(toBinary_helper(*sib.head(), sib.tail()))
                    );
    }
}


int main() {
    Heap<int> h;
    h = h.insert(1).insert(3).insert(4).insert(-100);
    BinaryHeap<int> bh = toBinary(h);
    bh.deleteMin();
    h.deleteMin();
}



