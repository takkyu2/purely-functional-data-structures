#include "WeightBiasedLeftistHeap.hpp"
#include <iostream>

int main() {
    WeightBiasedLeftistHeap<int> hp = WeightBiasedLeftistHeap<int>::Heapify({1,2,3,4});
    auto hp2 = hp.insert(3).insert(-10).insert(100).insert(1310).insert(0).insert(0);
    auto hp3 = hp.insert(4).insert(6);
    hp2.print(); std::cout << std::endl;
    hp3.print(); std::cout << std::endl;
    auto hpi = merge_iterative(hp2,hp3);
    hpi.print(); std::cout << std::endl;
}

