#include "BankersQueue.hpp"
#include <iostream>

int main() {
    BankersQueue<int> que;
    for (int i = 0; i < 30000; ++i) {
        que = snoc(i, que);
    }
    for (int i = 0; i < 10; ++i) {
        std::cout << que.head() << std::endl;
        que = que.tail();
    }
}

