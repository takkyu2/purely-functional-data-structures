#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <ranges>
#include "../utils/utils.hpp"

using namespace std::placeholders;

template <Ordered Elem>
class BinarySearchTree {
private:
    struct SubTree {
        SubTree(Elem x) : elem(x) { }
        SubTree(Elem x, std::shared_ptr<BinarySearchTree> l, std::shared_ptr<BinarySearchTree> r) 
            : elem(x), left(l), right(r) { }
        Elem elem;
        std::shared_ptr<BinarySearchTree> left, right;
    };
public:
    BinarySearchTree() = default;
    BinarySearchTree(SubTree t) : m_tree(t) { }
    BinarySearchTree(const BinarySearchTree& bst) = default;
    BinarySearchTree(Elem x)
        : m_tree(SubTree(
                    x, 
                    std::make_shared<BinarySearchTree>(BinarySearchTree::empty()), 
                    std::make_shared<BinarySearchTree>(BinarySearchTree::empty())
                )
                ) { }
    BinarySearchTree(Elem x, std::shared_ptr<BinarySearchTree> l, std::shared_ptr<BinarySearchTree> r)
        : m_tree(SubTree(x, l, r)) { }
    static BinarySearchTree empty() { return {};}
    bool member(Elem x) const {
        static auto visit_member = overloaded{
            [](Empty e, Elem x) {
                return false;
            },
            [](SubTree t, Elem x) {
                if (x < t.elem)
                    return t.left->member(x);
                else if (t.elem < x)
                    return t.right->member(x);
                else
                    return true;
            },
        };
        return std::visit(std::bind(visit_member, _1, x), m_tree);
    }
    BinarySearchTree insert(Elem x) const {
        static auto visit_insert = overloaded {
            [] (Empty e, Elem x) {
                return BinarySearchTree(x);
            },
            [] (SubTree t, Elem x) {
                if (x < t.elem)
                    return BinarySearchTree(t.elem, std::make_shared<BinarySearchTree>(t.left->insert(x)), t.right);
                else if (t.elem < x)
                    return BinarySearchTree(t.elem, t.left, std::make_shared<BinarySearchTree>(t.right->insert(x)));
                else
                    return BinarySearchTree(t);
            },
        };
        return std::visit(std::bind(visit_insert, _1, x), m_tree);
    }
    bool member_memoized(Elem x) const {
        if (std::holds_alternative<Empty>(m_tree))
            return false;
        else
            return member_memoized_helper(x, std::get<SubTree>(m_tree).elem);
    }
    BinarySearchTree insert_err(Elem x) const {
        static auto visit_insert = overloaded{
            [] (Empty e, Elem x) {
                return BinarySearchTree(x);
            },
            [] (SubTree t, Elem x) {
                if (x < t.elem)
                    return BinarySearchTree(t.elem, std::make_shared<BinarySearchTree>(t.left->insert_err(x)), t.right);
                else if (t.elem < x)
                    return BinarySearchTree(t.elem, t.left, std::make_shared<BinarySearchTree>(t.right->insert_err(x)));
                else
                    throw std::runtime_error("same node found");
            },
        };
        try {
            return std::visit(std::bind(visit_insert, _1, x), m_tree);
        } catch (std::runtime_error e) {
            std::cout << "copy avoided!" << std::endl;
            return *this;
        }
    }
    void print() const {
        static auto visit_print = overloaded {
            [](Empty e) { },
            [](SubTree t) {
                std::cout << t.elem << "(";
                t.left->print();
                std::cout << " : ";
                t.right->print();
                std::cout << ")";
            }
        };
        std::visit(visit_print, m_tree);
    }
    Elem getElem(Elem x) const {
        if (std::holds_alternative<Empty>(m_tree))
            throw std::runtime_error("element not found, empty");
        else
            return getElem_helper(x, std::get<SubTree>(m_tree).elem);
    }
private:
    bool member_memoized_helper(Elem x, Elem memo) const {
        static auto visit_member = overloaded {
            [](Empty e, Elem x, Elem memo) {
                return x == memo;
            },
            [](SubTree t, Elem x, Elem memo) {
                if (x < t.elem)
                    return t.left->member_memoized_helper(x, memo);
                else
                    return t.right->member_memoized_helper(x, t.elem);
            },
        };
        return std::visit(std::bind(visit_member, _1, x, memo), m_tree);
    }
    Elem getElem_helper(Elem x, Elem memo) const {
        static auto visit_member = overloaded {
            [](Empty e, Elem x, Elem memo) {
                if (x == memo)
                    return memo;
                else
                    throw std::runtime_error("element not found");
            },
            [](SubTree t, Elem x, Elem memo) {
                if (x < t.elem)
                    return t.left->getElem_helper(x, memo);
                else
                    return t.right->getElem_helper(x, t.elem);
            },
        };
        return std::visit(std::bind(visit_member, _1, x, memo), m_tree);
    }
    std::variant<Empty, SubTree> m_tree;
};

template <Ordered Key, typename Val>
class FiniteMap {
private:
    class kv_pair {
    public:
        kv_pair(Key k, Val v) : key(k), val(v) { }
        Key key;
        Val val;
        bool operator<(const kv_pair& rhs) const { return key < rhs.key; }
        bool operator==(const kv_pair& rhs) const { return key == rhs.key; }
        bool operator<=(const kv_pair& rhs) const { return key <= rhs.key; }
    };
    BinarySearchTree<kv_pair> m_bst;
public:
    FiniteMap() = default;
    FiniteMap(BinarySearchTree<kv_pair> b) : m_bst(b) { }
    FiniteMap empty() { return {}; }
    FiniteMap bind(Key k, Val v) {
        return m_bst.insert_err({k, v});
    }
    Val lookup(Key k) {
        return m_bst.getElem({k, Val{}}).val;
    }
};


int main() {
    BinarySearchTree<int> bst;
    auto bst2 = bst.insert_err(3).insert_err(3).insert_err(2).insert_err(1);
    bst2.print(); std::cout << std::endl;
    std::cout << bst2.member_memoized(3) << std::endl;
    std::cout << bst2.member_memoized(1) << std::endl;
    std::cout << bst2.member_memoized(5) << std::endl;
    std::cout << bst2.member(3) << std::endl;
    std::cout << bst2.member(1) << std::endl;
    std::cout << bst2.member(5) << std::endl;
    bst2 = bst.insert(3).insert(3).insert(2).insert(1);
    std::cout << std::endl;
    bst2.print(); std::cout << std::endl;
    std::cout << bst2.member_memoized(3) << std::endl;
    std::cout << bst2.member_memoized(1) << std::endl;
    std::cout << bst2.member_memoized(5) << std::endl;
    std::cout << bst2.member(3) << std::endl;
    std::cout << bst2.member(1) << std::endl;
    std::cout << bst2.member(5) << std::endl;
    FiniteMap<int, int> fm;
    fm = fm.bind(1,3);
    std::cout << fm.lookup(1) << std::endl;
    BinarySearchTree<int> bsst;
    for (auto elem: std::views::iota(1,20)) {
        bsst = bsst.insert_err(elem);
    }
    bsst.print(); std::cout << std::endl;
}
