#ifndef SKEWBINARYRANDOMACCESSLIST
#define SKEWBINARYRANDOMACCESSLIST
#include "../List/List.hpp"
#include <variant>

template <typename Elem>
class SkewBinaryRandomAccessList {
private:
    struct Tree;
    struct Leaf {
        Elem elem;
    };
    struct Node {
        Elem elem;
        std::shared_ptr<Tree> left, right;
    };
    struct Tree {
        Tree() = default;
        Tree(Leaf lf) : m_tree(lf) {}
        Tree(Node nd) : m_tree(nd) {}
        std::variant<Leaf, Node> m_tree;
        Elem lookupTree(int w, int i) const {
            if (w == 1 && i == 0 && std::holds_alternative<Leaf>(m_tree)) return std::get<Leaf>(m_tree).elem;
            else if (w == 1 && std::holds_alternative<Leaf>(m_tree)) throw std::runtime_error("SUBSCRIPT");
            else if (i == 0 && std::holds_alternative<Node>(m_tree)) return std::get<Node>(m_tree).elem;
            else {
                auto [elem, left, right] = std::get<Node>(m_tree);
                if (i <= w / 2) return left->lookupTree(w/2, i-1);
                else return right->lookupTree(w/2, i-1-w/2);
            }
        }
        Tree updateTree(int w, int i, Elem y) const {
            if (w == 1 && i == 0 && std::holds_alternative<Leaf>(m_tree)) return Leaf(y);
            else if (w == 1 && std::holds_alternative<Leaf>(m_tree)) throw std::runtime_error("SUBSCRIPT");
            else if (i == 0 && std::holds_alternative<Node>(m_tree)) {
                auto [elem, left, right] = std::get<Node>(m_tree);
                return Node(y,left,right);
            } else {
                auto [elem, left, right] = std::get<Node>(m_tree);
                if (i <= w / 2) return Node(elem, left->updateTree(w/2,i-1,y), right);
                else return Node(elem, left, right->updateTree(w/2,i-1-w/2,y));
            }
        }
    };
    using RList = ImList<std::tuple<int, std::shared_ptr<Tree>>>;
    RList m_list;
public:
    SkewBinaryRandomAccessList() = default;
    SkewBinaryRandomAccessList(RList lis) : m_list(lis) {}
    static SkewBinaryRandomAccessList empty() {
        return SkewBinaryRandomAccessList();
    }
    bool isEmpty() const {
        return m_list.isEmpty();
    }
    SkewBinaryRandomAccessList cons(Elem x) const {
        if (!m_list.isEmpty() && !m_list.tail().isEmpty()) {
            auto [w1, t1] = m_list.head();
            auto [w2, t2] = m_list.tail().head();
            if (w1 == w2) return m_list.tail().tail().cons(std::make_tuple(1+w1+w2, std::make_shared<Tree>(Tree(Node(x, t1, t2)))));
            else return m_list.cons({1, std::make_shared<Tree>(Tree(Leaf(x)))});
        } else {
            return m_list.cons({1, std::make_shared<Tree>(Tree(Leaf(x)))});
        }
    }
    friend SkewBinaryRandomAccessList cons(Elem x, const SkewBinaryRandomAccessList q) {
        return q.cons(x);
    }
    Elem head() const {
        if (isEmpty()) throw std::runtime_error("EMPTY");
        auto [w, t] = m_list.head();
        if (std::holds_alternative<Leaf>(t->m_tree)) {
            return std::get<Leaf>(t->m_tree).elem;
        } else {
            return std::get<Node>(t->m_tree).elem;
        }
    }
    SkewBinaryRandomAccessList tail() const {
        if (isEmpty()) throw std::runtime_error("EMPTY");
        auto [w, t] = m_list.head();
        if (std::holds_alternative<Leaf>(t->m_tree)) {
            return m_list.tail();
        } else {
            auto nd = std::get<Node>(t->m_tree);
            return m_list.tail().cons({w/2, nd.right}).cons({w/2, nd.left});
        }
    }
    Elem lookup(int i) const {
        if (isEmpty()) throw std::runtime_error("EMPTY");
        auto [w, t] = m_list.head();
        if (i < w) return t->lookupTree(w,i);
        else return SkewBinaryRandomAccessList(m_list.tail()).lookup(i-w);
    }
    SkewBinaryRandomAccessList update(int i, Elem y) const {
        if (isEmpty()) throw std::runtime_error("SUBSCRIPT");
        auto [w, t] = m_list.head();
        if (i < w) return m_list.tail().cons({w, t->updateTree(w,i,y)});
        else return SkewBinaryRandomAccessList(m_list.tail()).update(i-w,y).m_list.cons({w,t});
    }
};
#endif
