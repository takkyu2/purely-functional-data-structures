#include "Trie.hpp"
#include <iostream>

int main() {
    ImList<char> lis;
    lis = lis.cons('a');
    lis = lis.cons('b');
    lis = lis.cons('c');
    Trie<char, int> tr;
    tr = tr.bind(lis, 10);
    std::cout << tr.lookup(lis) << std::endl;
    lis = lis.cons('d');
    tr = tr.bind(lis, 18);
    std::cout << tr.lookup(lis) << std::endl;
}
