# Immutable-Data-Structures
Yet another implementation of the immutable data structures in Okasaki's textbook, "Purely Functional Data Structures", in C++.

## List of data structures

### Chapter 2
* [List](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/List.hpp)
* [Binary Search Tree](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Tree/BinarySearchTree.hpp)

### Chapter 3
* [Leftiest Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/LeftistHeap.hpp)
* [Weight Biased Leftist Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/WeightBiasedLeftistHeap.hpp)
* [Binomial Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/BinomialHeap.hpp)
* [Explicit Minimum Binomial Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/ExplicitMinBinomialHeap.hpp)
* [Red Black Tree](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Tree/RedBlackTree.hpp)
* [Red Black Tree with not-so-efficient-delete](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Tree/RedBlackTreeWithDel.hpp)

### Chapter 4
* [Stream (with O(n*k) insertion sort)](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/Stream.hpp)

### Chapter 5
* Queue
* [Deque](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/Deque.hpp)
* [Splay Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/SplayHeap.hpp)
* [Pairing Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/PairingHeap.hpp)

### Chapter 6
* [Bankers Queue](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/BankersQueue.hpp)
* [Lazy Binomial Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/LazyBinomialHeap.hpp)
* [Physicists Queue](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/PhysicistsQueue.hpp)
* [Bottom Up Merge sort](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/BottomUpMergeSort.hpp)
* [Lazy Pairing Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/LazyPairingHeap.hpp)

### Chapter 7
* [Real Time Queue](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/RealTimeQueue.hpp)
* [Scheduled Binomial Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/ScheduledBinomialHeap.hpp)
* [Scheduled Bottom Up Merge Sort](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/ScheduledBottomUpMergeSort.hpp)

### Chapter 8
* [Hood Melville Queue](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/HoodMelvilleRealTimeQueue.hpp)
* [Bankers Deque](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/BankersDeque.hpp)
* [Real Time Deque](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/RealTimeDeque.hpp)

### Chapter 9
* [Binary Random Access List](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/RandomAccessList/BinaryRandomAccessList.hpp)
* [Skew Binary Random Access List](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/RandomAccessList/BinaryRandomAccessList.hpp)
* [Skew Binomial Heap](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Heap/SkewBinomialHeap.hpp)

## Disclaimer
In Chapter 10 and 11, we need a polymorphic recursion.
Although the static typing system of C++ forbids a proliferation of infinite number of types at compile time (i.e., a genuine polymorphic recursion),
we can introduce the auxiliary cutoff of the recursion with constexpr if. If the depth of the recursion exceeds the cutoff, the runtime exception is thrown.

### Chapter 10
* [Alt Binary Random Access List](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/RandomAccessList/AltBinaryRandomAccessList.hpp)
* [Bootstrapped Queue](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/BootstrappedQueue.hpp)
* [Catenable List](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/CatenableList.hpp)
* [Trie](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/Tree/Trie.hpp)
* Hash Table
* Trie of Trees

### Chapter 11
* [Implicit Queue](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/ImplicitQueue.hpp)
* [Simple Catenable Deque](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/SimpleCatenableDeque.hpp)
* [Implicit Catenable Deque](https://github.com/takkyu2/Immutable-Data-Structures/blob/master/List/SimpleCatenableDeque.hpp)

## References
type erasure: [C++ Templates - The Complete Guide](http://www.tmplbook.com/)

lazy evaluation: [Functional Programming in C++](https://www.manning.com/books/functional-programming-in-c-plus-plus) 
