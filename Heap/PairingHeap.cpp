#include "PairingHeap.hpp"

int main() {
    Heap<int> h;
    h = h.insert(1).insert(3).insert(4).insert(-100);
    PairingHeap<int> bh = toBinary(h);
    bh.deleteMin();
    h.deleteMin();
}
