#include "ExplicitMinBinomialHeap.hpp"
#include <iostream>

int main() {
    ExplicitMinBinomialHeap<int> hp;
    hp = hp.insert(3).insert(4).insert(8);
    auto hp2 = hp.insert(10).insert(-100);
    std::cout << hp2.findMin() << std::endl;
    hp2 = hp2.deleteMin();
    hp2.print();
}

