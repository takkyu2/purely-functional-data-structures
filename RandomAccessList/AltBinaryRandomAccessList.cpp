#include "AltBinaryRandomAccessList.hpp"
#include <iostream>

int main() {
    AltBinaryRandomAccessList<int> vec;
    for (int i = 0; i < 100; ++i) {
        vec = vec.cons(i);
        /* vec = vec.cons(i); */
    }
    for (int i = 0; i < 100; ++i) {
        /* std::cout << vec.head() << std::endl; */
        std::cout << vec.lookup(i) << std::endl;
        /* vec = vec.tail(); */
    }
}
