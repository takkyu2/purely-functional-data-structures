#include "Deque.hpp"
#include <iostream>

int main() {
    Deque<int> que;
    for (int i = 0; i < 10; ++i) {
        que = cons(i, que);
    }
    for (int i = 20; i < 30; ++i) {
        que = snoc(i, que);
    }
    Deque<int> que2 = que;
    for (int i = 0; i < 20; ++i) {
        std::cout << que.head() << std::endl;
        que = que.tail();
    }
    std::cout << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < 20; ++i) {
        std::cout << que2.last() << std::endl;
        que2 = que2.init();
    }
}

