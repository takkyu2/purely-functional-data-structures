#include <string>
#include <memory>
#include <ranges>
#include <iterator>
#include <variant>
#include <functional>
#include <iostream>
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"
#include "Stream.hpp"

int main() {
    Stream<int> st, st2;
    for (int i = 0; i < 10; ++i) {
        st = cons(i, st);
    }
    for (int i = 10; i < 200; ++i) {
        st2 = cons(200-i, st2);
    }
    Stream<int> st3 = st + st2;
    Stream<int> st4 = st3.drop(3);
    Stream<int> st5 = st3.reverse();

    Stream<int> st2v = st3.sort();

    std::cout << std::endl;
    for (int i = 0; i < 20; ++i) {
        std::cout << st3.head() << std::endl;
        st3 = st3.tail();
    }
    std::cout << std::endl;
    for (int i = 0; i < 100; ++i) {
        std::cout << st2v.head() << std::endl;
        st2v = st2v.tail();
    }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << st4.head() << std::endl;
        st4 = st4.tail();
    }
    std::cout << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << st5.head() << std::endl;
        st5 = st5.tail();
    }
}
