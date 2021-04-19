#include "LeftistHeap.hpp"
#include <iostream>

int main() {
    LeftiestHeap<int> hp = LeftiestHeap<int>::LeftiestHeapify({1,2,3,4});
    auto hp2 = hp.insert(3).insert(-10).insert(100).insert(1310).insert(0).insert(0);
    auto hp3 = hp.insert(4).insert(6);
    hp2.print(); std::cout << std::endl;
    std::cout << hp2.findMin() << std::endl;
    hp2 = hp2.deleteMin();
    hp2 = hp2.deleteMin();
    hp3 = hp3.merge(hp2);
    hp3.print(); std::cout << std::endl;
    std::cout << hp2.findMin() << std::endl;
}

