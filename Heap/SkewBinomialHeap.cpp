#include "SkewBinomialHeap.hpp"
#include <iostream>

int main() {
    SkewBinomialHeap<int> h;
    h = h.insert(-10000).insert(1).insert(3).insert(4).insert(-100);
    while (!h.isEmpty()) {
        std::cout << h.findMin() << std::endl;
        h = h.deleteMin();
    }
}

