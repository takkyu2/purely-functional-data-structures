#include "AltBinaryRandomAccessList.hpp"
#include <iostream>

int main() {
    AltBinaryRandomAccessList<int> vec;
    std::cout << vec.isEmpty() << std::endl;
    auto vec2 = vec.cons(0);
    for (int i = 0; i < 1; ++i) {
        /* vec = vec.cons(i); */
        /* vec = vec.cons(i); */
    }
    /* for (int i = 0; i < 3; ++i) { */
    /*     /1* std::cout << vec.head() << std::endl; *1/ */
    /*     std::cout << vec.lookup(i) << std::endl; */
    /*     /1* vec = vec.tail(); *1/ */
    /* } */
}
