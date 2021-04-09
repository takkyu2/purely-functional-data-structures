#include "ScheduledBinomialHeap.hpp"
#include <iostream>

int main() {
    ScheduledBinomialHeap<int> hp;
    for (int i = 0; i <= 100; ++i) {
        hp = hp.insert(i);
    }
    while (!hp.isEmpty()) {
        std::cout << hp.findMin() << std::endl;
        hp = hp.fastdeleteMin();
    }
}
