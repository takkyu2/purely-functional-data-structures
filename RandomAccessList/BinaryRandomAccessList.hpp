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
        std::variant<Leaf, Node> m_tree;
    public:
        int size() const {
            if (std::holds_alternative<Leaf>(m_tree)) {
                return 1;
            } else {
                return std::get<Tree>(m_tree).sz;
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
    };
    using RList = ImList<Digit>;
    RList m_rlist;
public:
    BinaryRandomAccessList() = default;
    BinaryRandomAccessList(Rlist rlist)
        : m_rlist(rlist) {}
    static BinaryRandomAccessList empty() {
        return BinaryRandomAccessList();
    }
    bool isEmpty() const {
        return m_rlist.isEmpty();
    }
    static Tree link(const Tree t1, const Tree t2) {
        return Node(t1.size()+t2.size(), std::make_shared<Tree>(t1), std::make_shared<Tree>(t2));
    }
    BinaryRandomAccessList consTree(const Tree t) const {
        if (isEmpty()) return ImList<Digit>();
    }
};
