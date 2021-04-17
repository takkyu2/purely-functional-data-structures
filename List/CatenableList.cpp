#include <iostream>
#include <vector>
#include "CatenableList.hpp"

int main() {
    CatenableListBase<int> lis;
    for (int i = 0; i < 100; ++i) {
        lis = lis.cons(i);
    }
    CatenableListBase<int> lis2;
    for (int i = 0; i < 100; ++i) {
        lis2 = lis2.snoc(i);
    }
    auto lis3 = lis + lis2;
    for (int i = 0; i < 200; ++i) {
        std::cout << lis3.head() << std::endl;
        lis3 = lis3.tail();
    }
}

