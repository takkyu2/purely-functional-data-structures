#include "BinomialHeap.hpp"
#include <iostream>

int main() {
    BinomialHeap<int> hp;
    hp = hp.insert(3).insert(4).insert(8);
    auto hp2 = hp.insert(10).insert(-100);
    std::cout << hp2.findMin() << std::endl;
    std::cout << hp2.findMin_direct() << std::endl;
    hp2 = hp2.deleteMin();
    hp2.print();
}

