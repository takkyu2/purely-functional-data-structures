#include "BinarySearchTree.hpp"
#include <iostream>

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

