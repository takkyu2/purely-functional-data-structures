#include "SimpleCatenableDeque.hpp"
#include <iostream>
#include <cassert>

int main() {
    SimpleCatenableDequeBase<int> que;
    for (int i = 0; i < 100; ++i) {
        que = que.cons(i);
    }
    SimpleCatenableDequeBase<int> que2;
    for (int i = 0; i < 100; ++i) {
        que2 = que2.snoc(i);
    }
    auto que3 = que + que2;
    for (int i = 0; i < 200; ++i) {
        std::cout << que3.head() << std::endl;
        que3 = que3.tail();
    }
    for (int i = 0; i < 100; ++i) {
        assert(que.last() == i);
        que = que.init();
    }
    que = SimpleCatenableDequeBase<int>::empty();
    for (int i = 0; i < 100; ++i) {
        que = que.cons(i);
    }
    for (int i = 0; i < 100; ++i) {
        assert(que.last() == i);
        que = que.init();
    }
}


