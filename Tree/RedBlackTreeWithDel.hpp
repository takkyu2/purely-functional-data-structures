#ifndef REDBLACKTREEWITHDEL
#define REDBLACKTREEWITHDEL
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
class SizeAwareRBTree;

template <Ordered Elem>
class RBTree;

template <Ordered Elem>
RBTree<Elem> FromOrdList(ImList<Elem>, int);

template <Ordered Elem>
RBTree<Elem> FromOrdList_helper(ImList<Elem>&, int, int);

template <Ordered Elem>
class RBTree {
public:
    template <Ordered T> 
    friend class SizeAwareRBTree;

    class Red {};
    class Black {};
    using Color = std::variant<Red, Black>;
    class Left {};
    class Right {};
    class Neither {};
    using Dir = std::variant<Left, Right, Neither>;
private:
    struct SubTree {
        SubTree(Elem x) : elem(x) { }
        SubTree(Color c, std::pair<Elem,bool> x, std::shared_ptr<RBTree> l, std::shared_ptr<RBTree> r) 
            : color(c), elem(x.first), left(l), right(r), val_tag(x.second) { }
        Color color;
        Elem elem;
        std::shared_ptr<RBTree> left, right;
        bool val_tag = true;
    };
    std::variant<Empty, SubTree> m_tree;
public:
    RBTree() = default;
    RBTree(SubTree t) : m_tree(t) { }
    RBTree(const RBTree& bst) = default;
    RBTree(Elem x)
        : m_tree(SubTree(
                    Red {},
                    {x, true}, 
                    std::make_shared<RBTree>(RBTree::empty()), 
                    std::make_shared<RBTree>(RBTree::empty())
                )
                ) { }
    RBTree(Color c, std::pair<Elem,bool> x, std::shared_ptr<RBTree> l, std::shared_ptr<RBTree> r)
        : m_tree(SubTree(c, x, l, r)) { }
    static RBTree empty() { return {};}
    bool isEmpty() {
        return std::holds_alternative<Empty>(m_tree);
    }

    friend RBTree<Elem> FromOrdList_helper<Elem>(ImList<Elem>& head, int sz, int depth);
    friend RBTree<Elem> FromOrdList<Elem>(ImList<Elem> head, int sz);

    bool member(Elem x) const {
        if (std::holds_alternative<Empty>(m_tree))
            return false;
        else
            return member_memoized_helper(x, std::get<SubTree>(m_tree).elem, std::get<SubTree>(m_tree).val_tag);
    }
    RBTree insert(Elem x) const {
        try {
            Dir d = Neither {};
            bool inserted = true;
            auto tr = std::get<SubTree>(insert_helper(x, d, inserted).m_tree);
            tr.color = Black {};
            return tr;
        } catch (std::runtime_error e) {
            std::cout << "copy avoided!" << std::endl;
            return *this;
        }
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
            if (cont.val_tag) {
                std::cerr << "o ";
            } else {
                std::cerr << "x ";
            }
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
                if (cont.val_tag) {
                    std::cerr << "o ";
                } else {
                    std::cerr << "x ";
                }
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
        else {
            bool val_tag_memo = true;
            return getElem_helper(x, std::get<SubTree>(m_tree).elem, val_tag_memo);
        }
    }
private:
    friend RBTree lbalance(Color c, std::pair<Elem,bool> elem, std::shared_ptr<RBTree> left, std::shared_ptr<RBTree> right, Dir& d) {
        static auto visit_balance = overloaded {
            [] (Black c, std::pair<Elem,bool> elem, SubTree left, std::shared_ptr<RBTree> right, Left d) {
                static auto visit_balance_left = overloaded {
                    [] (Red c, std::pair<Elem,bool> elem, std::pair<Elem,bool> l_elem, SubTree l_left, std::shared_ptr<RBTree> l_right, std::shared_ptr<RBTree> right) {
                        auto [ll_color, ll_elem1, ll_left, ll_right, ll_elem2] = l_left;
                        std::pair<Elem, bool> ll_elem {ll_elem1, ll_elem2};
                        if (std::holds_alternative<Black>(ll_color)) throw std::runtime_error("not matched!");
                        return RBTree(Red{}
                                ,l_elem
                                ,std::make_shared<RBTree>(Black{}, ll_elem, ll_left, ll_right)
                                ,std::make_shared<RBTree>(Black{}, elem, l_right, right)
                                );
                    },
                    [] (auto c, std::pair<Elem,bool> e, std::pair<Elem,bool> el, auto ll, std::shared_ptr<RBTree> lr, std::shared_ptr<RBTree> r) {
                        throw std::runtime_error("not matched!");
                        return RBTree();
                    }
                };
                auto [l_color, l_elem1, l_left, l_right, l_elem2] = left;
                std::pair<Elem, bool> l_elem {l_elem1, l_elem2};
                return std::visit(std::bind(visit_balance_left, _1, elem, l_elem, _2, l_right, right), l_color, l_left->m_tree); 
            },
            [] (Black c, std::pair<Elem,bool> elem, SubTree left, std::shared_ptr<RBTree> right, Right d) {
                static auto visit_balance_right = overloaded {
                    [] (Red c, std::pair<Elem,bool> elem, std::pair<Elem,bool> l_elem, std::shared_ptr<RBTree> l_left, SubTree l_right, std::shared_ptr<RBTree> right) {
                        auto [lr_color, lr_elem1, lr_left, lr_right, lr_elem2] = l_right;
                        std::pair<Elem, bool> lr_elem {lr_elem1, lr_elem2};
                        if (std::holds_alternative<Black>(lr_color)) throw std::runtime_error("not matched!");
                        return RBTree(Red{}
                                ,lr_elem
                                ,std::make_shared<RBTree>(Black{}, l_elem, l_left, lr_left)
                                ,std::make_shared<RBTree>(Black{}, elem, lr_right, right)
                                );
                    },
                    [] (auto c, std::pair<Elem,bool> e, std::pair<Elem,bool> el, std::shared_ptr<RBTree>ll, auto lr, std::shared_ptr<RBTree> r) {
                        throw std::runtime_error("not matched!");
                        return RBTree();
                    }
                };
                auto [l_color, l_elem1, l_left, l_right, l_elem2] = left;
                std::pair<Elem, bool> l_elem {l_elem1, l_elem2};
                return std::visit(std::bind(visit_balance_right, _1, elem, l_elem, l_left, _2, right), l_color, l_right->m_tree); 
            },
            [] (auto c, std::pair<Elem,bool> elem, auto left, std::shared_ptr<RBTree> right, auto d) {
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
    friend RBTree rbalance(Color c, std::pair<Elem,bool> elem, std::shared_ptr<RBTree> left, std::shared_ptr<RBTree> right, Dir& d) {
        static auto visit_balance = overloaded {
            [] (Black c, std::pair<Elem,bool> elem, std::shared_ptr<RBTree> left, SubTree right, Left d) {
                static auto visit_balance_left = overloaded {
                    [] (Red c, std::pair<Elem,bool> elem, std::pair<Elem,bool> r_elem, SubTree r_left, std::shared_ptr<RBTree> r_right, std::shared_ptr<RBTree> left) {
                        auto [rl_color, rl_elem1, rl_left, rl_right, rl_elem2] = r_left;
                        std::pair<Elem, bool> rl_elem {rl_elem1, rl_elem2};
                        if (std::holds_alternative<Black>(rl_color)) throw std::runtime_error("not matched!");
                        return RBTree(Red{}
                                ,rl_elem
                                ,std::make_shared<RBTree>(Black{}, elem, left, rl_left)
                                ,std::make_shared<RBTree>(Black{}, r_elem, rl_right, r_right)
                                );
                    },
                    [] (auto c, std::pair<Elem,bool> e, std::pair<Elem,bool> el, auto rl, std::shared_ptr<RBTree> rr, std::shared_ptr<RBTree> l) {
                        throw std::runtime_error("not matched!");
                        return RBTree();
                    }
                };
                auto [r_color, r_elem1, r_left, r_right, r_elem2] = right;
                std::pair<Elem, bool> r_elem {r_elem1, r_elem2};
                return std::visit(std::bind(visit_balance_left, _1, elem, r_elem, _2, r_right, left), r_color, r_left->m_tree); 
            },
            [] (Black c, std::pair<Elem,bool> elem, std::shared_ptr<RBTree> left, SubTree right, Right d) {
                static auto visit_balance_right = overloaded {
                    [] (Red c, std::pair<Elem,bool> elem, std::pair<Elem,bool> r_elem, std::shared_ptr<RBTree> r_left, SubTree r_right, std::shared_ptr<RBTree> left) {
                        auto [rr_color, rr_elem1, rr_left, rr_right, rr_elem2] = r_right;
                        std::pair<Elem, bool> rr_elem {rr_elem1, rr_elem2};
                        if (std::holds_alternative<Black>(rr_color)) throw std::runtime_error("not matched!");
                        return RBTree(Red{}
                                ,r_elem
                                ,std::make_shared<RBTree>(Black{}, elem, left, r_left)
                                ,std::make_shared<RBTree>(Black{}, rr_elem, rr_left, rr_right)
                                );
                    },
                    [] (auto c, std::pair<Elem,bool> e, std::pair<Elem,bool> el, std::shared_ptr<RBTree> rl, auto rr, std::shared_ptr<RBTree> l) {
                        throw std::runtime_error("not matched!");
                        return RBTree();
                    }
                };
                auto [r_color, r_elem1, r_left, r_right, r_elem2] = right;
                std::pair<Elem, bool> r_elem {r_elem1, r_elem2};
                return std::visit(std::bind(visit_balance_right, _1, elem, r_elem, r_left, _2, left), r_color, r_right->m_tree); 
            },
            [] (auto c, std::pair<Elem,bool> elem, std::shared_ptr<RBTree> left, auto right, auto d) {
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
    bool member_memoized_helper(Elem x, Elem memo, bool val_tag_memo) const {
        static auto visit_member = overloaded {
            [](Empty e, Elem x, Elem memo, bool val_tag_memo) {
                return val_tag_memo && (x == memo);
            },
            [](SubTree t, Elem x, Elem memo, bool val_tag_memo) {
                if (x < t.elem)
                    return t.left->member_memoized_helper(x, memo, val_tag_memo);
                else
                    return t.right->member_memoized_helper(x, t.elem, t.val_tag);
            },
        };
        return std::visit(std::bind(visit_member, _1, x, memo, val_tag_memo), m_tree);
    }
    Elem getElem_helper(Elem x, Elem memo, bool val_tag_memo) const {
        static auto visit_member = overloaded {
            [](Empty e, Elem x, Elem memo, bool val_tag_memo) {
                if ((x == memo) && val_tag_memo)
                    return memo;
                else
                    throw std::runtime_error("element not found");
            },
            [](SubTree t, Elem x, Elem memo, bool val_tag_memo) {
                if (x < t.elem)
                    return t.left->getElem_helper(x, memo, val_tag_memo);
                else
                    return t.right->getElem_helper(x, t.elem, t.val_tag);
            },
        };
        return std::visit(std::bind(visit_member, _1, x, memo, val_tag_memo), m_tree);
    }
    RBTree insert_helper(Elem x, Dir& d, bool& inserted) const {
        static auto visit_insert = overloaded {
            [] (Empty e, Elem x, Dir& d, bool& inserted) {
                return RBTree(x);
            },
            [] (SubTree t, Elem x, Dir& d, bool& inserted) {
                if (x < t.elem) {
                    auto tr = lbalance(t.color, {t.elem, t.val_tag}, std::make_shared<RBTree>(t.left->insert_helper(x, d, inserted)), t.right, d);
                    d = Left {};
                    return tr;
                } else if (t.elem < x) {
                    auto tr = rbalance(t.color, {t.elem, t.val_tag}, t.left, std::make_shared<RBTree>(t.right->insert_helper(x, d, inserted)), d);
                    d = Right {};
                    return tr;
                } else if (t.val_tag) {
                    throw std::runtime_error("same node found");
                } else {
                    t.val_tag = true;
                    inserted = false;
                    return RBTree(t);
                }
            },
        };
        return std::visit(std::bind(visit_insert, _1, x, std::ref(d), std::ref(inserted)), m_tree);
    }
    RBTree delete_helper(Elem x) const {
        static auto visit_delete = overloaded {
            [] (Empty e, Elem x) {
                throw std::runtime_error("not found");
                return RBTree();
            },
            [] (SubTree t, Elem x) {
                if (x < t.elem) {
                    return RBTree(t.color, {t.elem, t.val_tag}, std::make_shared<RBTree>(t.left->delete_helper(x)), t.right);
                } else if (t.elem < x) {
                    return RBTree(t.color, {t.elem, t.val_tag}, t.left, std::make_shared<RBTree>(t.right->delete_helper(x)));
                } else if (!t.val_tag) {
                    throw std::runtime_error("already deleted");
                } else {
                    t.val_tag = false;
                    return RBTree(t);
                }
            },
        };
        return std::visit(std::bind(visit_delete, _1, x), m_tree);
    }
    void ToOrdList(ImList<Elem>& lis) {
        if (isEmpty()) return;
        auto t = std::get<SubTree>(m_tree);
        t.right->ToOrdList(lis);
        if (t.val_tag) lis = cons(t.elem, lis);
        t.left->ToOrdList(lis);
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

    return RBTree<Elem>(c, std::make_pair(rootelem,true), std::make_shared<RBTree<Elem>>(left), std::make_shared<RBTree<Elem>>(right));
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

template <Ordered Elem>
class SizeAwareRBTree {
public:
    SizeAwareRBTree() = default;
    SizeAwareRBTree(RBTree<Elem> tr, int t, int v)
        : m_rbtree(tr), total_nodes(t), valid_nodes(v) { }
    bool member(Elem x) const {
        return m_rbtree.member(x);
    }
    int size() const {
        return valid_nodes;
    }
    void print() const {
        return m_rbtree.print();
    }
    void print_bfs() const {
        return m_rbtree.print_bfs();
    }
    Elem getElem() const {
        return m_rbtree.getElem();
    }
    SizeAwareRBTree insert(Elem x) const {
        try {
            Dir d = Neither {};
            bool inserted = true;
            auto tr = std::get<SubTree>(m_rbtree.insert_helper(x, d, inserted).m_tree);
            tr.color = Black {};
            return SizeAwareRBTree(RBTree<Elem>(tr), total_nodes + (inserted ? 1 : 0), valid_nodes + 1);
        } catch (std::runtime_error e) {
            std::cout << "copy avoided!" << std::endl;
            return *this;
        }
    }
    SizeAwareRBTree del_elem(Elem x) const {
        try {
            return rebuild(m_rbtree.delete_helper(x), total_nodes, valid_nodes - 1);
        } catch (std::runtime_error e) {
            std::cout << "delete avoided!" << std::endl;
            return *this;
        }
    }
private:
    SizeAwareRBTree rebuild(RBTree<Elem> tr, int total_nodes, int valid_nodes) const {
        static constexpr int th_num = 0;
        if (total_nodes > th_num && total_nodes >= 2*valid_nodes) {
            ImList<Elem> lis;
            tr.ToOrdList(lis);
            return {FromOrdList(lis), valid_nodes, valid_nodes};
        } else {
            return {tr, total_nodes, valid_nodes};
        }
    }

    using Red = typename RBTree<Elem>::Red;
    using Black = typename RBTree<Elem>::Black;
    using Color = typename RBTree<Elem>::Color;
    using Neither = typename RBTree<Elem>::Neither;
    using Left = typename RBTree<Elem>::Left;
    using Right = typename RBTree<Elem>::Right;
    using Dir = typename RBTree<Elem>::Dir;
    using SubTree = typename RBTree<Elem>::SubTree;

    RBTree<Elem> m_rbtree;
    int total_nodes = 0;
    int valid_nodes = 0;
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
    FiniteMap empty() { return {}; }
    FiniteMap bind(Key k, Val v) {
        return m_bst.insert({k, v});
    }
    Val lookup(Key k) {
        return m_bst.getElem({k, Val{}}).val;
    }
};

int main() {
    SizeAwareRBTree<int> bst;
    for (int i = 0; i < 20; ++i) {
        bst = bst.insert(i);
    }
    bst.print_bfs();

    for (int i = 10; i < 20; ++i) {
        bst = bst.del_elem(i);
    }
    bst.print_bfs();
}

#endif
