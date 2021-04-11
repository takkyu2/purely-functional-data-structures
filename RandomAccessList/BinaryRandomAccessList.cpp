#include "BinaryRandomAccessList.hpp"
#include <iostream>

int main() {
    BinaryRandomAccessList<int> vec;
    for (int i = 0; i < 100; ++i) {
        vec = vec.cons(i);
    }
    /* for (int i = 0; i < 100; ++i) { */
    /*     std::cout << vec.lookup(i) << std::endl; */
    /* } */
}
