#include "ImplicitQueue.hpp"
#include <iostream>

int main() {
    ImplicitQueueBase<int> que;
    for (int i = 0; i < 10; ++i) {
        que = que.snoc(i);
    }
    while (!que.isEmpty()) {
        std::cout << que.head() << std::endl;
        que = que.tail();
    }
}


