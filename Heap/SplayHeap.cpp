#include <iostream>
#include <concepts>
#include <vector>
#include <variant>
#include <memory>
#include <functional>
#include <stack>
#include "../utils/utils.hpp"
#include "../List/List.hpp"

using namespace std::placeholders;

template <Ordered Elem>
class Heap {
private:
    class Tree {
    public:
        Tree() = default;
        Tree(Elem e, std::shared_ptr<Heap> left, std::shared_ptr<Heap> right) : elem(e), left(left), right(right) { }
        Elem elem;
        std::shared_ptr<Heap> left, right;
        int getelem() const {
            return elem;
        }
    };
    std::variant<Empty, Tree> m_heap;
public:
    Heap() = default;
    Heap(const Tree& t) : m_heap(t) { }
    Heap(Elem e, std::shared_ptr<Heap> left, std::shared_ptr<Heap> right) : m_heap(Tree(e, left, right)) { }
    bool isEmpty() const {
        return std::holds_alternative<Empty>(m_heap);
    }
    Heap insert(Elem piv) const {
        auto [small, big] = partition(piv);
        return Tree(piv, std::make_shared<Heap>(small), std::make_shared<Heap>(big));
    }
    Heap smaller(Elem piv) const {
        static const auto visit_smaller = overloaded {
            [](Elem piv, Empty e) {
                return Heap();
            },
            [](Elem piv, Tree t) {
                static const auto visit_smaller_2 = overloaded {
                    [](Elem piv, Elem par_elem, std::shared_ptr<Heap> left, Empty right) {
                        return Heap(par_elem, left, std::make_shared<Heap>(Heap()));
                    },
                    [](Elem piv, Elem par_elem, std::shared_ptr<Heap> left, Tree right) {
                        if (piv < right.getelem()) 
                            return Heap(par_elem, left, std::make_shared<Heap>(right.left->smaller(piv)));
                        else
                            return Heap(right.getelem(), 
                                    std::make_shared<Heap>(par_elem, left, right.left),
                                    std::make_shared<Heap>(right.right->bigger(piv))
                                    );
                    },
                };
                if (piv < t.getelem()) 
                    return t.left->smaller(piv);
                else
                    return std::visit(std::bind_front(visit_smaller_2, piv, t.getelem(), t.left), t.right->m_heap);
            },
        };
        return std::visit(std::bind_front(visit_smaller, piv), m_heap);
    }
    Heap bigger(Elem piv) const {
        static const auto visit_biggger = overloaded {
            [](Elem piv, Empty e) {
                return Heap();
            },
            [](Elem piv, Tree t) {
                static const auto visit_bigger_2 = overloaded {
                    [](Elem piv, Elem par_elem, std::shared_ptr<Heap> right, Empty left) {
                        return Heap(par_elem, std::make_shared<Heap>(Heap()), right);
                    },
                    [](Elem piv, Elem par_elem, std::shared_ptr<Heap> right, Tree left) {
                        if (left.getelem() <= piv) 
                            return Heap(par_elem, std::make_shared<Heap>(left.right->bigger(piv)), right);
                        else
                            return Heap(left.getelem(), std::make_shared<Heap>(left.left->bigger(piv)), 
                                    std::make_shared<Heap>(par_elem, left.right, right));
                    },
                };
                if (t.getelem() <= piv) 
                    return t.right->bigger(piv);
                else
                    return std::visit(std::bind_front(visit_bigger_2, piv, t.getelem(), t.right), t.left->m_heap);
            },
        };
        return std::visit(std::bind_front(visit_biggger, piv), m_heap);
    }
    std::pair<Heap, Heap> partition(Elem piv) const {
        static const auto visit_partition = overloaded {
            [](Elem piv, Empty e) {
                return std::make_pair(Heap(), Heap());
            },
            [](Elem piv, Tree t) {
                static const auto visit_partition_r = overloaded {
                    [](Elem piv, Tree par, Empty right) {
                        return std::make_pair(Heap(par), Heap());
                    },
                    [](Elem piv, Tree par, Tree right) {
                        if (right.getelem() <= piv) {
                            auto [small, big] = right.right->partition(piv);
                            return std::make_pair(
                                    Heap(right.getelem(), 
                                        std::make_shared<Heap>(Heap(par.getelem(), par.left, right.left)),
                                        std::make_shared<Heap>(small)
                                        ),
                                        big);
                        } else {
                            auto [small, big] = right.left->partition(piv);
                            return std::make_pair(
                                    Heap(par.getelem(), par.left, std::make_shared<Heap>(small)),
                                    Heap(right.getelem(), std::make_shared<Heap>(big), right.right)
                                    );
                        }
                    },
                };
                static const auto visit_partition_l = overloaded {
                    [](Elem piv, Tree par, Empty left) {
                        return std::make_pair(Heap(), Heap(par));
                    },
                    [](Elem piv, Tree par, Tree left) {
                        if (left.getelem() <= piv) {
                            auto [small, big] = left.right->partition(piv);
                            return std::make_pair(
                                    Heap(left.getelem(), left.left, std::make_shared<Heap>(small)),
                                    Heap(par.getelem(), std::make_shared<Heap>(big), par.right)
                                    );
                        } else {
                            auto [small, big] = left.left->partition(piv);
                            return std::make_pair(
                                    small,
                                    Heap(left.getelem(), std::make_shared<Heap>(big), 
                                        std::make_shared<Heap>(Heap(
                                            par.getelem(),
                                            left.right,
                                            par.right
                                            ))
                                        )
                                    );
                        }
                    },

                };
                if (t.getelem() <= piv) 
                    return std::visit(std::bind_front(visit_partition_r, piv, t), t.right->m_heap);
                else
                    return std::visit(std::bind_front(visit_partition_l, piv, t), t.left->m_heap);
            }
        };
        return std::visit(std::bind_front(visit_partition, piv), m_heap);
    }
    Elem findMin() {
        if (isEmpty()) throw std::runtime_error("findMin no element");
        auto t = std::get<Tree>(m_heap);
        if (std::holds_alternative<Empty>(t.left->m_heap))
            return t.getelem();
        else
            return t.left->findMin();
    }
    Heap deleteMin() {
        static auto visit_deletemin = overloaded {
            [](Empty e) {
                return Heap();
            },
            [](Tree t) {
                static auto visit_deletemin_2 = overloaded {
                    [](Tree par, Empty e) {
                        return *(par.right);
                    },
                    [](Tree par, Tree t2) {
                        static auto visit_deletemin_3 = overloaded {
                            [](Tree par, Tree t2, Empty e) {
                                return Heap(par.getelem(), t2.right, par.right);
                            },
                            [](Tree par, Tree t2, Tree t2_left) {
                                return Heap(t2.getelem(), std::make_shared<Heap>(t2.left->deleteMin()), 
                                        std::make_shared<Heap>(Heap(par.getelem(), t2.right, par.right)));
                            },
                        };
                        return std::visit(std::bind_front(visit_deletemin_3, par, t2), t2.left->m_heap);
                    }
                };
                return std::visit(std::bind_front(visit_deletemin_2, t), t.left->m_heap);
            },
        };
        return std::visit(visit_deletemin, m_heap);
    }
    friend void dfs(Heap h, std::vector<int>& v) {
        if (h.isEmpty()) return;
        auto t = std::get<Tree>(h.m_heap);
        dfs(*(t.left),v);
        v.push_back(t.getelem());
        dfs(*(t.right),v);
    }
};

std::vector<int> sortWithHeap(std::vector<int> v) {
    Heap<int> h;
    for (auto elem : v) {
        h = h.insert(elem);
    }
    std::vector<int> ans;
    dfs(h, ans);
    return ans;
}


int main() {
    Heap<int> hp;
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

