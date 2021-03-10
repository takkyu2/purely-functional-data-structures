#include <iostream>
#include <concepts>
#include <variant>
#include <memory>

template <typename T>
concept Ordered = requires(T a, T b) {
    {a < b} -> std::convertible_to<bool>;
    {a == b} -> std::convertible_to<bool>;
    {a <= b} -> std::convertible_to<bool>;
};

struct Empty {};

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
    static BinarySearchTree empty() { return {};}
    bool member(Elem x) const {
        static auto visit_member = overloaded{
            [](Empty e, Elem x) {
                return false;
            },
            [](SubTree t, Elem x) {
                if (x < t.elem)
                    return t.left.member(x);
                else if (t.elem < x)
                    return t.right.member(x);
                else
                    return true;
            },
        };
        return std::visit(visit_member, m_tree, x);
    }
    BinarySearchTree insert(Elem x) const {
        static auto visit_insert = overloaded{
            [] (Empty e, Elem x) {
                return SubTree(x);
            },
            [] (SubTree t, Elem x) {
                if (x < t.elem)
                    return BinarySearchTree(t.elem, std::make_shared(t.left.insert(x)), t.right);
            },
        };
        return std::visit(visit_insert, m_tree, x);
    }
private:
    std::variant<Empty, SubTree> m_tree;
};
