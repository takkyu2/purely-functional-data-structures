#include <memory>
#include <variant>

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
    };
    struct Node {
        int rk;
        std::shared_ptr<Tree> left, right;
    };
    struct Zero { };
    struct One {
        
    };
public:

};
