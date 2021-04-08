#include "BottomUpMergeSort.hpp"
#include <iostream>

int main() {
    BottomUpMergeSort<int> e = BottomUpMergeSort<int>::empty();
    for (int i = 100; i >= 0; --i) {
        e = e.add(i);
    }
    ImList<int> se = e.sort();

    while (!se.isEmpty()) {
        std::cout << se.head() << std::endl;
        se = se.tail();
    }
}

