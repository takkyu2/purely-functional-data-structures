#include "RealTimeDeque.hpp"
#include <iostream>
#include <cassert>

int main() {
    RealTimeDeque<int> que;
    for (int i = 0; i < 100; ++i) {
        que = que.cons(i);
    }
    for (int i = 0; i < 100; ++i) {
        assert(que.last() == i);
        que = que.init();
    }
    que = RealTimeDeque<int>::empty();
    for (int i = 0; i < 100; ++i) {
        que = cons(i,que);
    }
    for (int i = 0; i < 100; ++i) {
        assert(que.last() == i);
        que = que.init();
    }
}

