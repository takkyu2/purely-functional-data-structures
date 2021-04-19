#include "LazyBinomialHeap.hpp"
#include <iostream>

int main() {
    LazyBinomialHeap<int> hp;
    hp = hp.insert(3).insert(4).insert(8);
    auto hp2 = hp.insert(10).insert(-100);
    hp.print();
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    hp2.print();
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    hp2 = hp2.deleteMin();
    hp2.print();
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    auto hp3 = merge(hp, hp2);
    hp3.print();
}

