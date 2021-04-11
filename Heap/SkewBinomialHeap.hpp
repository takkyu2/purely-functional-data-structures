#include "../List/List.hpp"

template <typename Elem>
class SkewBinomialHeap {
private:
    struct Tree {
        int rk;
        Elem elem;
        ImList<Elem> xs;
        ImList<std::shared_ptr<Tree>> m_treelist;
        int rank() const {
            return rk;
        }
        Elem root() const {
            return elem;
        }
        friend Tree link(std::shared_ptr<Tree> t1, std::shared_ptr<Tree> t2) {
            if (t1->elem <= t2->elem) 
                return Tree(t1->rk+1, t1->elem, t1->xs, t1->m_treelist.cons(t2));
            else 
                return Tree(t1->rk+1, t2->elem, t2->xs, t2->m_treelist.cons(t1));
        }
        friend Tree skewLink(Elem x, std::shared_ptr<Tree> t1, std::shared_ptr<Tree> t2) {
            auto [r, y, ys, c] = link(t1, t2);
            if (x <= y) return Tree(r, x, ys.cons(y), c);
            else return Tree(r, y, ys.cons(x), c);
        }
        friend ImList<std::shared_ptr<Tree>> insTree(std::shared_ptr<Tree> t, ImList<std::shared_ptr<Tree>> ts) {
            if (ts.isEmpty()) return ts.cons(t);
            else {
                if (t->rk < ts.head()->rk) return ts.cons(t);
                else return insTree(std::make_shared<Tree>(link(t, ts.head())), ts.tail());
            }
        }
        friend ImList<std::shared_ptr<Tree>> mergeTrees(ImList<std::shared_ptr<Tree>> ts1, ImList<std::shared_ptr<Tree>> ts2) {
            if (ts2.isEmpty()) return ts1;
            else if (ts1.isEmpty()) return ts2;
            else {
                if (ts1.head()->rk < ts2.head()->rk) {
                    return mergeTrees(ts1.tail(), ts2).cons(ts1.head());
                } else if (ts2.head()->rk < ts1.head()->rk) {
                    return mergeTrees(ts1, ts2.tail()).cons(ts2.head());
                } else {
                    return insTree(std::make_shared<Tree>(link(ts1.head(), ts2.head())), mergeTrees(ts1.tail(), ts2.tail()));
                }
            }
        }
    };
    using Heap = ImList<std::shared_ptr<Tree>>;
    Heap m_heap;
    friend Heap normalize(const Heap ts) {
        if (ts.isEmpty()) return ts;
        else return insTree(ts.head(), ts.tail());
    }
public:
    SkewBinomialHeap() = default;
    SkewBinomialHeap(Heap hp) : m_heap(hp) {}
    static SkewBinomialHeap empty() {
        return SkewBinomialHeap();
    }
    bool isEmpty() const {
        return m_heap.isEmpty();
    }
    SkewBinomialHeap insert(Elem x) const {
        if (!m_heap.isEmpty() && !m_heap.tail().isEmpty()) {
            if (m_heap.head()->rk == m_heap.tail().head()->rk) {
                return m_heap.tail().tail().cons(std::make_shared<Tree>(skewLink(x, m_heap.head(), m_heap.tail().head())));
            } else {
                return m_heap.cons(std::make_shared<Tree>(Tree(0, x, {}, {})));
            }
        } else {
            return m_heap.cons(std::make_shared<Tree>(Tree(0, x, {}, {})));
        }
    }
    friend Heap merge(SkewBinomialHeap h1, SkewBinomialHeap h2) {
        return mergeTrees(normalize(h1.m_heap), normalize(h2.m_heap));
    }
    std::tuple<std::shared_ptr<Tree>, SkewBinomialHeap> removeMinTree() const {
        if (isEmpty()) throw std::runtime_error("EMPTY");
        else if (m_heap.tail().isEmpty()) return {m_heap.head(), {}};
        else {
            auto [tp, tsp] = SkewBinomialHeap(m_heap.tail()).removeMinTree();
            if (m_heap.head()->root() <= tp->root())
                return {m_heap.head(), m_heap.tail()};
            else
                return {tp, tsp.m_heap.cons(m_heap.head())};
        }
    }
    Elem findMin() const {
        auto [t, tmp] = removeMinTree();
        return t->root();
    }
    friend SkewBinomialHeap insertAll(const ImList<Elem> xs, const SkewBinomialHeap h) {
        if (xs.isEmpty()) return h;
        return insertAll(xs.tail(), h.insert(xs.head()));
    }
    SkewBinomialHeap deleteMin() const {
        auto [nd, ts2] = removeMinTree();
        auto [tmp, x, xs, ts1] = *nd;
        return insertAll(xs, merge(ts1.rev(), ts2));
    }
};
