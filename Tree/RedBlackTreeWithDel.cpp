#include "RedBlackTreeWithDel.hpp"

int main() {
    SizeAwareRBTree<int> bst;
    for (int i = 0; i < 20; ++i) {
        bst = bst.insert(i);
    }
    bst.print_bfs();

    for (int i = 10; i < 20; ++i) {
        bst = bst.del_elem(i);
    }
    bst.print_bfs();
}
