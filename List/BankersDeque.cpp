#include "BankersDeque.hpp"
#include <iostream>
#include <cassert>

int main() {
    BankersDeque<int> que;
    for (int i = 0; i < 100; ++i) {
        que = cons(i,que);
    }
    for (int i = 0; i < 100; ++i) {
        assert(que.last() == i);
        que = que.init();
    }
    que = BankersDeque<int>::empty();
    for (int i = 0; i < 100; ++i) {
        que = cons(i,que);
    }
    for (int i = 0; i < 100; ++i) {
        assert(que.last() == i);
        que = que.init();
    }
}
