#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>

using namespace std::placeholders;
struct Empty {};

template <typename T>
concept Ordered = requires(T a, T b) {
    {a < b} -> std::convertible_to<bool>;
    {a == b} -> std::convertible_to<bool>;
    {a <= b} -> std::convertible_to<bool>;
};

// use inheritance to overload operator()
template <typename... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

// type deduction guide
template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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
            [](Empty e) {
                return;
            },
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
private:
    std::variant<Empty, SubTree> m_tree;
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
}
