#include "BootstrappedQueue.hpp"
#include <iostream>

int main() {
    BootstrappedQueueBase<int> que;
    for (int i = 0; i < 10000; ++i) {
        que = que.snoc(i);
    }

    while (!que.isEmpty()) {
        std::cout << que.head() << std::endl;
        que = que.tail();
    }
}


