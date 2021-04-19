#ifndef SPLAYHEAP
#define SPLAYHEAP
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
class SplayHeap {
private:
    class Tree {
    public:
        Tree() = default;
        Tree(Elem e, std::shared_ptr<SplayHeap> left, std::shared_ptr<SplayHeap> right) : elem(e), left(left), right(right) { }
        Elem elem;
        std::shared_ptr<SplayHeap> left, right;
        int getelem() const {
            return elem;
        }
    };
    std::variant<Empty, Tree> m_heap;
public:
    SplayHeap() = default;
    SplayHeap(const Tree& t) : m_heap(t) { }
    SplayHeap(Elem e, std::shared_ptr<SplayHeap> left, std::shared_ptr<SplayHeap> right) : m_heap(Tree(e, left, right)) { }
    bool isEmpty() const {
        return std::holds_alternative<Empty>(m_heap);
    }
    SplayHeap insert(Elem piv) const {
        auto [small, big] = partition(piv);
        return Tree(piv, std::make_shared<SplayHeap>(small), std::make_shared<SplayHeap>(big));
    }
    SplayHeap smaller(Elem piv) const {
        static const auto visit_smaller = overloaded {
            [](Elem piv, Empty e) {
                return SplayHeap();
            },
            [](Elem piv, Tree t) {
                static const auto visit_smaller_2 = overloaded {
                    [](Elem piv, Elem par_elem, std::shared_ptr<SplayHeap> left, Empty right) {
                        return SplayHeap(par_elem, left, std::make_shared<SplayHeap>(SplayHeap()));
                    },
                    [](Elem piv, Elem par_elem, std::shared_ptr<SplayHeap> left, Tree right) {
                        if (piv < right.getelem()) 
                            return SplayHeap(par_elem, left, std::make_shared<SplayHeap>(right.left->smaller(piv)));
                        else
                            return SplayHeap(right.getelem(), 
                                    std::make_shared<SplayHeap>(par_elem, left, right.left),
                                    std::make_shared<SplayHeap>(right.right->bigger(piv))
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
    SplayHeap bigger(Elem piv) const {
        static const auto visit_biggger = overloaded {
            [](Elem piv, Empty e) {
                return SplayHeap();
            },
            [](Elem piv, Tree t) {
                static const auto visit_bigger_2 = overloaded {
                    [](Elem piv, Elem par_elem, std::shared_ptr<SplayHeap> right, Empty left) {
                        return SplayHeap(par_elem, std::make_shared<SplayHeap>(SplayHeap()), right);
                    },
                    [](Elem piv, Elem par_elem, std::shared_ptr<SplayHeap> right, Tree left) {
                        if (left.getelem() <= piv) 
                            return SplayHeap(par_elem, std::make_shared<SplayHeap>(left.right->bigger(piv)), right);
                        else
                            return SplayHeap(left.getelem(), std::make_shared<SplayHeap>(left.left->bigger(piv)), 
                                    std::make_shared<SplayHeap>(par_elem, left.right, right));
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
    std::pair<SplayHeap, SplayHeap> partition(Elem piv) const {
        static const auto visit_partition = overloaded {
            [](Elem piv, Empty e) {
                return std::make_pair(SplayHeap(), SplayHeap());
            },
            [](Elem piv, Tree t) {
                static const auto visit_partition_r = overloaded {
                    [](Elem piv, Tree par, Empty right) {
                        return std::make_pair(SplayHeap(par), SplayHeap());
                    },
                    [](Elem piv, Tree par, Tree right) {
                        if (right.getelem() <= piv) {
                            auto [small, big] = right.right->partition(piv);
                            return std::make_pair(
                                    SplayHeap(right.getelem(), 
                                        std::make_shared<SplayHeap>(SplayHeap(par.getelem(), par.left, right.left)),
                                        std::make_shared<SplayHeap>(small)
                                        ),
                                        big);
                        } else {
                            auto [small, big] = right.left->partition(piv);
                            return std::make_pair(
                                    SplayHeap(par.getelem(), par.left, std::make_shared<SplayHeap>(small)),
                                    SplayHeap(right.getelem(), std::make_shared<SplayHeap>(big), right.right)
                                    );
                        }
                    },
                };
                static const auto visit_partition_l = overloaded {
                    [](Elem piv, Tree par, Empty left) {
                        return std::make_pair(SplayHeap(), SplayHeap(par));
                    },
                    [](Elem piv, Tree par, Tree left) {
                        if (left.getelem() <= piv) {
                            auto [small, big] = left.right->partition(piv);
                            return std::make_pair(
                                    SplayHeap(left.getelem(), left.left, std::make_shared<SplayHeap>(small)),
                                    SplayHeap(par.getelem(), std::make_shared<SplayHeap>(big), par.right)
                                    );
                        } else {
                            auto [small, big] = left.left->partition(piv);
                            return std::make_pair(
                                    small,
                                    SplayHeap(left.getelem(), std::make_shared<SplayHeap>(big), 
                                        std::make_shared<SplayHeap>(SplayHeap(
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
    SplayHeap deleteMin() {
        static auto visit_deletemin = overloaded {
            [](Empty e) {
                return SplayHeap();
            },
            [](Tree t) {
                static auto visit_deletemin_2 = overloaded {
                    [](Tree par, Empty e) {
                        return *(par.right);
                    },
                    [](Tree par, Tree t2) {
                        static auto visit_deletemin_3 = overloaded {
                            [](Tree par, Tree t2, Empty e) {
                                return SplayHeap(par.getelem(), t2.right, par.right);
                            },
                            [](Tree par, Tree t2, Tree t2_left) {
                                return SplayHeap(t2.getelem(), std::make_shared<SplayHeap>(t2.left->deleteMin()), 
                                        std::make_shared<SplayHeap>(SplayHeap(par.getelem(), t2.right, par.right)));
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
    friend void dfs(SplayHeap h, std::vector<int>& v) {
        if (h.isEmpty()) return;
        auto t = std::get<Tree>(h.m_heap);
        dfs(*(t.left),v);
        v.push_back(t.getelem());
        dfs(*(t.right),v);
    }
};

#endif
