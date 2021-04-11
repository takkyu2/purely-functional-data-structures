#include <memory>
#include <variant>
#include "../List/List.hpp"

template <typename Elem>
class BinaryRandomAccessList {
private:
    struct Leaf;
    struct Tree;
    struct Node;

    struct Leaf {
        Elem elem;
    };
    class Tree {
    private:
        std::variant<Leaf, Node> m_tree;
    public:
        Tree() = default;
        Tree(Leaf lf) : m_tree(lf) {}
        Tree(Node nd) : m_tree(nd) {}
        int size() const {
            if (std::holds_alternative<Leaf>(m_tree)) {
                return 1;
            } else {
                return std::get<Node>(m_tree).sz;
            }
        }
        Elem lookupTree(int i) const {
            if (i == 0 && std::holds_alternative<Leaf>(m_tree)) return std::get<Leaf>(m_tree).elem;
            else {
                auto [sz, left, right] = std::get<Node>(m_tree);
                if (i < sz / 2) return left->lookupTree(i);
                else return right->lookupTree(i - sz / 2);
            }
        }
        Tree updateTree(int i, Elem y) const {
            if (i == 0 && std::holds_alternative<Leaf>(m_tree)) return Leaf(y);
            else {
                auto [sz, left, right] = std::get<Node>(m_tree);
                if (i < sz / 2) return Node(sz, left.updateTree(i, y), right);
                else return Node(sz, left, right.updateTree(i - sz / 2, y));
            }
        }
    };
    struct Node {
        Node() = default;
        Node(int sz, std::shared_ptr<Tree> left, std::shared_ptr<Tree> right)
            : sz(sz), left(left), right(right) {}
        int sz;
        std::shared_ptr<Tree> left, right;
    };
    struct Zero {};
    struct One {
        std::shared_ptr<Tree> tr;
    };
    struct Digit {
        std::variant<Zero, One> dig;
        Digit() = default;
        Digit(Tree t) : dig(One(std::make_shared<Tree>(t))) {}
        Digit(std::shared_ptr<Tree> t) : dig(One(t)) {}
    };
    using RList = ImList<Digit>;
    RList m_rlist;
    bool headIsOne() const {
        return std::holds_alternative<One>(m_rlist.head().dig);
    }
    bool headIsZero() const {
        return std::holds_alternative<Zero>(m_rlist.head().dig);
    }
    One headGetOne() const {
        return std::get<One>(m_rlist.head().dig);
    }
    Zero headGetZero() const {
        return std::get<Zero>(m_rlist.head().dig);
    }
    BinaryRandomAccessList rlistTail() const {
        return RList(m_rlist.tail());
    }
    static std::shared_ptr<Tree> link(std::shared_ptr<Tree> t1, std::shared_ptr<Tree> t2) {
        return std::make_shared<Tree>(Tree(Node(t1->size()+t2->size(), t1, t2)));
    }
    BinaryRandomAccessList consTree(const std::shared_ptr<Tree> t) const {
        if (isEmpty()) return RList();
        else if (headIsZero()) return RList(m_rlist.tail().cons(Digit(t)));
        else return RList(rlistTail().consTree(link(t, headGetOne().tr)).m_rlist.cons(Digit()));
    }
    std::tuple<std::shared_ptr<Tree>, BinaryRandomAccessList> unconsTree() const {
        if (rlistTail().isEmpty() && headIsOne()) return {headGetOne().tr, BinaryRandomAccessList()};
        else if (headIsOne()) return {headGetOne().tr, cons(Zero(), rlistTail())};
        else {
            auto [t, tsp] = tail().unconsTree();
            auto nd = std::get<Node>(*t);
            return {nd.left, cons(One(nd.right), tsp)};
        }
    }
public:
    BinaryRandomAccessList() = default;
    BinaryRandomAccessList(RList rlist)
        : m_rlist(rlist) {}
    static BinaryRandomAccessList empty() {
        return BinaryRandomAccessList();
    }
    bool isEmpty() const {
        return m_rlist.isEmpty();
    }
    BinaryRandomAccessList cons(Elem x) const {
        return consTree(std::make_shared<Tree>(Tree(Leaf(x))));
    }
    /* friend BinaryRandomAccessList cons(Elem x, const BinaryRandomAccessList q) { */
    /*     return q.cons(x); */
    /* } */
    Elem head() const {
        auto [lf, tmp] = unconsTree();
        return std::get<Leaf>(lf).elem;
    }
    BinaryRandomAccessList tail() const {
        auto [tmp, tsp] = unconsTree();
        return tsp;
    }
    Elem lookup(int i) const {
        if (headIsZero()) return rlistTail().lookup(i);
        else {
            auto t = headGetOne().tr;
            if (i < t->size()) return t->lookupTree(i);
            else return rlistTail().lookup(i - t->size());
        }
    }
    BinaryRandomAccessList update(int i, Elem y) const {
        if (headIsZero()) return RList(cons(Zero(), rlistTail().update(i,y)));
        else {
            auto t = headGetOne();
            if (i < t->size()) return RList(cons(One(t->updateTree(i, y)), rlistTail()));
            else return RList(cons(One(t), rlistTail().update(i - t->size(), y)));
        }
    }
};
