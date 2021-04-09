#include "LazyPairingHeap.hpp"
#include <iostream>

int main() {
    Heap<int> h;
    h = h.insert(1).insert(3).insert(4).insert(-100);
    while (!h.isEmpty()) {
        std::cout << h.findMin() << std::endl;
        h = h.deleteMin();
    }
}
