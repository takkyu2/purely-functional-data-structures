#include "SplayHeap.hpp"
#include <iostream>
#include <vector>

std::vector<int> sortWithHeap(std::vector<int> v) {
    SplayHeap<int> h;
    for (auto elem : v) {
        h = h.insert(elem);
    }
    std::vector<int> ans;
    dfs(h, ans);
    return ans;
}


int main() {
    SplayHeap<int> hp;
    hp = hp.insert(3).insert(3).insert(4).insert(8);
    auto hp2 = hp.insert(10).insert(-100);
    std::cout << hp2.findMin() << std::endl;
    hp2 = hp2.deleteMin();
    std::cout << hp2.findMin() << std::endl;
    hp2 = hp2.deleteMin();
    std::cout << hp2.findMin() << std::endl;
    hp2 = hp2.deleteMin();
    std::cout << hp2.findMin() << std::endl;

    std::vector<int> v {3,2,1,4,5};
    v = sortWithHeap(v);
    for (auto elem: v) {
        std::cout << elem << std::endl;
    }
}


