#include "TrieOfTrees.hpp"

int main() {
    TrieOfTrees<int, int> tr;
    Tree<int> bst {};
    bst = bst.insert(3);
    bst.print(); std::cout << std::endl;
    bst = bst.insert(2);
    bst.print(); std::cout << std::endl;
    std::cerr << std::endl;
    bst = bst.insert(1);
    bst.print_bfs(); std::cout << std::endl;
    RBTree<int> bst2;
    for (int elem = 0; elem < 300; ++elem) {
        bst2 = bst2.insert(elem);
    }
    bst2.print_bfs(); std::cout << std::endl;
    bst2.print(); std::cout << std::endl;
    std::cout << bst2.member(122) << std::endl;
    std::cout << bst2.member(200) << std::endl;


    ImList<int> lis;
    for (int i = 80; i >= 0; --i) {
        lis = lis.cons(i);
    }
    auto ttt = FromOrdList(lis);
    ttt.print_bfs();

    tr = tr.bind(ttt, 10);
}
