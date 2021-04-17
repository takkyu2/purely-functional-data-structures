#include "AltBinaryRandomAccessList.hpp"
#include <iostream>

int main() {
    AltBinaryRandomAccessListBase<double> vec;
    for (int i = 0; i < 100000; ++i) {
        vec = vec.cons(12.3*i);
        /* vec = vec.cons(i); */
    }
    for (int i = 0; i < 100000; ++i) {
        /* std::cout << vec.head() << std::endl; */
        std::cout << vec.lookup(i) << std::endl;
        /* vec = vec.tail(); */
    }
}
