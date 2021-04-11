#include "SkewBinaryRandomAccessList.hpp"
#include <iostream>

int main() {
    SkewBinaryRandomAccessList<int> vec;
    for (int i = 0; i < 100; ++i) {
        /* vec = vec.cons(i); */
        vec = cons(i, vec);
    }
    for (int i = 0; i < 100; ++i) {
        /* std::cout << vec.head() << std::endl; */
        std::cout << vec.lookup(i) << std::endl;
        /* vec = vec.tail(); */
    }
}

