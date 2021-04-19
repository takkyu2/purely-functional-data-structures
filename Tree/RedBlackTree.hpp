#ifndef REDBLACKTREE
#define REDBLACKTREE
#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <ranges>
#include <queue>
#include "../List/List.hpp"
#include "../utils/utils.hpp"

using namespace std::placeholders;

template <Ordered Elem>
class RBTree;

template <Ordered Elem>
RBTree<Elem> FromOrdList(ImList<Elem>, int);

template <Ordered Elem>
RBTree<Elem> FromOrdList_helper(ImList<Elem>&, int, int);

template <Ordered Elem>
class RBTree {
public:
    class Red {};
    class Black {};
    using Color = std::variant<Red, Black>;
private:
    class Left {};
    class Right {};
    class Neither {};
    using Dir = std::variant<Left, Right, Neither>;
    struct SubTree {
        SubTree(Elem x) : elem(x) { }
        SubTree(Color c, Elem x, std::shared_ptr<RBTree> l, std::shared_ptr<RBTree> r) 
            : color(c), elem(x), left(l), right(r) { }
        Color color;
        Elem elem;
        std::shared_ptr<RBTree> left, right;
    };
public:
    RBTree() = default;
    RBTree(SubTree t) : m_tree(t) { }
    RBTree(const RBTree& bst) = default;
    RBTree(Elem x)
        : m_tree(SubTree(
                    Red {},
                    x, 
                    std::make_shared<RBTree>(RBTree::empty()), 
                    std::make_shared<RBTree>(RBTree::empty())
                )
                ) { }
    RBTree(Color c, Elem x, std::shared_ptr<RBTree> l, std::shared_ptr<RBTree> r)
        : m_tree(SubTree(c, x, l, r)) { }
    static RBTree empty() { return {};}

    friend RBTree<Elem> FromOrdList_helper<Elem>(ImList<Elem>& head, int sz, int depth);
    friend RBTree<Elem> FromOrdList<Elem>(ImList<Elem> head, int sz);

    std::tuple<Elem, std::shared_ptr<RBTree>, std::shared_ptr<RBTree>> getNode() const {
        return {m_tree.x, m_tree.l, m_tree.r};
    }
    bool member(Elem x) const {
        if (std::holds_alternative<Empty>(m_tree))
            return false;
        else
            return member_memoized_helper(x, std::get<SubTree>(m_tree).elem);
    }
    RBTree insert(Elem x) const {
        try {
            Dir d = Neither {};
            auto tr = std::get<SubTree>(insert_helper(x, d).m_tree);
            tr.color = Black {};
            return tr;
        } catch (std::runtime_error e) {
            std::cout << "copy avoided!" << std::endl;
            return *this;
        }
    }
    RBTree insert_helper(Elem x, Dir& d) const {
        static auto visit_insert = overloaded {
            [] (Empty e, Elem x, Dir& d) {
                return RBTree(x);
            },
            [] (SubTree t, Elem x, Dir& d) {
                if (x < t.elem) {
                    auto tr = lbalance(t.color, t.elem, std::make_shared<RBTree>(t.left->insert_helper(x, d)), t.right, d);
                    d = Left {};
                    return tr;
                } else if (t.elem < x) {
                    auto tr = rbalance(t.color, t.elem, t.left, std::make_shared<RBTree>(t.right->insert_helper(x, d)), d);
                    d = Right {};
                    return tr;
                }
                else
                    throw std::runtime_error("same node found");
            },
        };
        return std::visit(std::bind(visit_insert, _1, x, std::ref(d)), m_tree);
    }
    void print() const {
        static auto visit_print = overloaded {
            [](Empty e) { },
            [](SubTree t) {
                if (std::holds_alternative<Red>(t.color))
                    std::cout << "R ";
                else
                    std::cout << "B ";
                std::cout << t.elem << "(";
                t.left->print();
                std::cout << " : ";
                t.right->print();
                std::cout << ")";
            }
        };
        std::visit(visit_print, m_tree);
    }
    void print_bfs() const {
        std::queue<std::shared_ptr<RBTree>> que;
        if (std::holds_alternative<Empty>(m_tree)) {
            std::cerr << "e\n";
        } else {
            auto cont = std::get<SubTree>(m_tree);
            if (std::holds_alternative<Black>(cont.color)) {
                std::cerr << "B";
            } else {
                std::cerr << "R";
            }
            std::cerr << cont.elem << std::endl;
            que.push(cont.left); que.push(cont.right);
            que.push(nullptr);
        }
        while (que.size()) {
            std::shared_ptr<RBTree> sp = que.front();
            que.pop();
            if (!sp) {
                std::cerr << std::endl;
                if (que.size() > 0) que.push(nullptr);
                continue;
            }
            if (std::holds_alternative<Empty>(sp->m_tree)) {
                std::cerr << "e ";
            } else {
                auto cont = std::get<SubTree>(sp->m_tree);
                if (std::holds_alternative<Black>(cont.color)) {
                    std::cerr << "B";
                } else {
                    std::cerr << "R";
                }
                std::cerr << cont.elem << " ";
                que.push(cont.left); que.push(cont.right);
            }
        }
        std::cerr << std::endl;
    }
    Elem getElem(Elem x) const {
        if (std::holds_alternative<Empty>(m_tree))
            throw std::runtime_error("element not found, empty");
        else
            return getElem_helper(x, std::get<SubTree>(m_tree).elem);
    }
private:
    friend RBTree lbalance(Color c, Elem elem, std::shared_ptr<RBTree> left, std::shared_ptr<RBTree> right, Dir& d) {
        static auto visit_balance = overloaded {
            [] (Black c, Elem elem, SubTree left, std::shared_ptr<RBTree> right, Left d) {
                static auto visit_balance_left = overloaded {
                    [] (Red c, Elem elem, Elem l_elem, SubTree l_left, std::shared_ptr<RBTree> l_right, std::shared_ptr<RBTree> right) {
                        auto [ll_color, ll_elem, ll_left, ll_right] = l_left;
                        if (std::holds_alternative<Black>(ll_color)) throw std::runtime_error("not matched!");
                        return RBTree(Red{}
                                ,l_elem
                                ,std::make_shared<RBTree>(Black{}, ll_elem, ll_left, ll_right)
                                ,std::make_shared<RBTree>(Black{}, elem, l_right, right)
                                );
                    },
                    [] (auto c, Elem e, Elem el, auto ll, std::shared_ptr<RBTree> lr, std::shared_ptr<RBTree> r) {
                        throw std::runtime_error("not matched!");
                        return RBTree();
                    }
                };
                auto [l_color, l_elem, l_left, l_right] = left;
                return std::visit(std::bind(visit_balance_left, _1, elem, l_elem, _2, l_right, right), l_color, l_left->m_tree); 
            },
            [] (Black c, Elem elem, SubTree left, std::shared_ptr<RBTree> right, Right d) {
                static auto visit_balance_right = overloaded {
                    [] (Red c, Elem elem, Elem l_elem, std::shared_ptr<RBTree> l_left, SubTree l_right, std::shared_ptr<RBTree> right) {
                        auto [lr_color, lr_elem, lr_left, lr_right] = l_right;
                        if (std::holds_alternative<Black>(lr_color)) throw std::runtime_error("not matched!");
                        return RBTree(Red{}
                                ,lr_elem
                                ,std::make_shared<RBTree>(Black{}, l_elem, l_left, lr_left)
                                ,std::make_shared<RBTree>(Black{}, elem, lr_right, right)
                                );
                    },
                    [] (auto c, Elem e, Elem el, std::shared_ptr<RBTree>ll, auto lr, std::shared_ptr<RBTree> r) {
                        throw std::runtime_error("not matched!");
                        return RBTree();
                    }
                };
                auto [l_color, l_elem, l_left, l_right] = left;
                return std::visit(std::bind(visit_balance_right, _1, elem, l_elem, l_left, _2, right), l_color, l_right->m_tree); 
            },
            [] (auto c, Elem elem, auto left, std::shared_ptr<RBTree> right, auto d) {
                throw std::runtime_error("not matched!");
                return RBTree();
            },
        };
        try {
            return std::visit(std::bind(visit_balance, _1, elem, _2, right, _3), c, left->m_tree, d); 
        } catch (std::runtime_error e) {
            return RBTree(c, elem, left, right);
        }
    }
    friend RBTree rbalance(Color c, Elem elem, std::shared_ptr<RBTree> left, std::shared_ptr<RBTree> right, Dir& d) {
        static auto visit_balance = overloaded {
            [] (Black c, Elem elem, std::shared_ptr<RBTree> left, SubTree right, Left d) {
                static auto visit_balance_left = overloaded {
                    [] (Red c, Elem elem, Elem r_elem, SubTree r_left, std::shared_ptr<RBTree> r_right, std::shared_ptr<RBTree> left) {
                        auto [rl_color, rl_elem, rl_left, rl_right] = r_left;
                        if (std::holds_alternative<Black>(rl_color)) throw std::runtime_error("not matched!");
                        return RBTree(Red{}
                                ,rl_elem
                                ,std::make_shared<RBTree>(Black{}, elem, left, rl_left)
                                ,std::make_shared<RBTree>(Black{}, r_elem, rl_right, r_right)
                                );
                    },
                    [] (auto c, Elem e, Elem el, auto rl, std::shared_ptr<RBTree> rr, std::shared_ptr<RBTree> l) {
                        throw std::runtime_error("not matched!");
                        return RBTree();
                    }
                };
                auto [r_color, r_elem, r_left, r_right] = right;
                return std::visit(std::bind(visit_balance_left, _1, elem, r_elem, _2, r_right, left), r_color, r_left->m_tree); 
            },
            [] (Black c, Elem elem, std::shared_ptr<RBTree> left, SubTree right, Right d) {
                static auto visit_balance_right = overloaded {
                    [] (Red c, Elem elem, Elem r_elem, std::shared_ptr<RBTree> r_left, SubTree r_right, std::shared_ptr<RBTree> left) {
                        auto [rr_color, rr_elem, rr_left, rr_right] = r_right;
                        if (std::holds_alternative<Black>(rr_color)) throw std::runtime_error("not matched!");
                        return RBTree(Red{}
                                ,r_elem
                                ,std::make_shared<RBTree>(Black{}, elem, left, r_left)
                                ,std::make_shared<RBTree>(Black{}, rr_elem, rr_left, rr_right)
                                );
                    },
                    [] (auto c, Elem e, Elem el, std::shared_ptr<RBTree> rl, auto rr, std::shared_ptr<RBTree> l) {
                        throw std::runtime_error("not matched!");
                        return RBTree();
                    }
                };
                auto [r_color, r_elem, r_left, r_right] = right;
                return std::visit(std::bind(visit_balance_right, _1, elem, r_elem, r_left, _2, left), r_color, r_right->m_tree); 
            },
            [] (auto c, Elem elem, std::shared_ptr<RBTree> left, auto right, auto d) {
                throw std::runtime_error("not matched!");
                return RBTree();
            },
        };
        try {
            return std::visit(std::bind(visit_balance, _1, elem, left, _2, _3), c, right->m_tree, d); 
        } catch (std::runtime_error e) {
            return RBTree(c, elem, left, right);
        }
    }
    bool member_memoized_helper(Elem x, Elem memo) const {
        static auto visit_member = overloaded {
            [](Empty e, Elem x, Elem memo) {
                return x == memo;
            },
            [](SubTree t, Elem x, Elem memo) {
                if (x < t.elem)
                    return t.left->member_memoized_helper(x, memo);
                else
                    return t.right->member_memoized_helper(x, t.elem);
            },
        };
        return std::visit(std::bind(visit_member, _1, x, memo), m_tree);
    }
    Elem getElem_helper(Elem x, Elem memo) const {
        static auto visit_member = overloaded {
            [](Empty e, Elem x, Elem memo) {
                if (x == memo)
                    return memo;
                else
                    throw std::runtime_error("element not found");
            },
            [](SubTree t, Elem x, Elem memo) {
                if (x < t.elem)
                    return t.left->getElem_helper(x, memo);
                else
                    return t.right->getElem_helper(x, t.elem);
            },
        };
        return std::visit(std::bind(visit_member, _1, x, memo), m_tree);
    }
    std::variant<Empty, SubTree> m_tree;
};

template <Ordered Key, typename Val>
class FiniteMap {
private:
    class kv_pair {
    public:
        kv_pair(Key k, Val v) : key(k), val(v) { }
        Key key;
        Val val;
        bool operator<(const kv_pair& rhs) const { return key < rhs.key; }
        bool operator==(const kv_pair& rhs) const { return key == rhs.key; }
        bool operator<=(const kv_pair& rhs) const { return key <= rhs.key; }
    };
    RBTree<kv_pair> m_bst;
public:
    FiniteMap() = default;
    FiniteMap(RBTree<kv_pair> b) : m_bst(b) { }
    FiniteMap empty() const { return {}; }
    FiniteMap bind(Key k, Val v) const {
        return m_bst.insert({k, v});
    }
    Val lookup(Key k) const {
        return m_bst.getElem({k, Val{}}).val;
    }
};

template <Ordered Elem>
RBTree<Elem> FromOrdList_helper(ImList<Elem>& head, int sz, int depth) {
    using Red = typename RBTree<Elem>::Red;
    using Black = typename RBTree<Elem>::Black;
    using Color = typename RBTree<Elem>::Color;

    if (sz <= 0) return RBTree<Elem>();
    RBTree<Elem> left = FromOrdList_helper(head, sz/2, depth-1);

    Elem rootelem = head.head();
    Color c = depth == 0 ? Color(Red {}) : Color(Black {});
    head = head.tail();

    RBTree<Elem> right = FromOrdList_helper(head, sz - 1 - sz/2, depth-1);

    return RBTree<Elem>(c, rootelem, std::make_shared<RBTree<Elem>>(left), std::make_shared<RBTree<Elem>>(right));
}
template <Ordered Elem>
RBTree<Elem> FromOrdList(ImList<Elem> head) {
    int sz = head.size();
    if (sz == 0) 
        return RBTree<Elem>();
    int depth = 0, twopow = 1;
    while (twopow - 1 <= sz) {
        depth++;
        twopow *= 2;
    }
    depth--;
    return FromOrdList_helper(head, sz, depth);
}
#endif
