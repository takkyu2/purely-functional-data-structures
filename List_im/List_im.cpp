#include <iostream>
#include <vector>
#include "List_im.hpp"

template <typename T>
ImList<ImList<T>> suffixes(ImList<T> xs) {
    return xs.isEmpty() ? ImList<ImList<T>>() : suffixes(xs.tail()).cons(xs);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, ImList<T> xs) {
    return xs.isEmpty() ? os << "()" : os << "(" << xs.head() << ":" << xs.tail() << ")";
}


int main() {
    ImList<int> xs = ImList<int>({1,2,3,4});
    auto x = std::ranges::end(xs);
    /* for (auto elem: xs | std::views::take(2)) */
    /*     std::cout << elem << std::endl; */
    std::cout << suffixes(xs) << std::endl;
}
