#include "RealTimeQueue.hpp"
#include <iostream>

int main() {
    RealTimeQueue<int> que;
    for (int i = 0; i < 100; ++i) {
        que = que.snoc(i);
    }
    while (!que.isEmpty()) {
        std::cout << que.head() << std::endl;
        que = que.tail();
    }
}
