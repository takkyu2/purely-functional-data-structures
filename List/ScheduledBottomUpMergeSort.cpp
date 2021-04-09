#include "ScheduledBottomUpMergeSort.hpp"
#include <iostream>

int main() {
    ScheduledBottomUpMergeSort<int> ms;
    for (int i = 100; i >= 0; --i) {
        ms = ms.add(i);
    }
    ImList<int> sorted = ms.sort();
    while (!sorted.isEmpty()) {
        std::cout << sorted.head() << std::endl;
        sorted = sorted.tail();
    }
}
