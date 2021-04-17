#include "RedBlackTree.hpp"
#include <variant>
#include <memory>

template <typename Elem>
using Tree = RBTree<Elem>;

template <typename BaseKey, typename Val>
class TrieOfTrees {
private:

    using Key = Tree<BaseKey>;
    std::optional<Val> m_flag;
    FiniteMap<BaseKey, std::shared_ptr<TrieOfTrees<BaseKey, std::shared_ptr<TrieOfTrees<BaseKey, Val>>>>> m_treelist;
public:
    TrieOfTrees() = default;
    TrieOfTrees(Val x, FiniteMap<BaseKey, std::shared_ptr<TrieOfTrees<BaseKey, std::shared_ptr<TrieOfTrees<BaseKey, Val>>>>> m_treelist)
        : m_flag(x), m_treelist(m_treelist) {}
    TrieOfTrees(std::optional<Val> x, FiniteMap<BaseKey, std::shared_ptr<TrieOfTrees<BaseKey, std::shared_ptr<TrieOfTrees<BaseKey, Val>>>>> m_treelist)
        : m_flag(x), m_treelist(m_treelist) {}
    static TrieOfTrees empty() {
        return TrieOfTrees();
    }
    TrieOfTrees lookup(Tree<BaseKey> tr) const {
        if (tr.isEmpty()) {
            if (!m_flag.has_value()) {
                throw std::runtime_error("NOTFOUND");
            } else {
                return m_flag.value();
            }
        } else {
            auto [k, left, right] = tr.getNode();
            return m_treelist(k).lookup(left).lookup(right);
        }
    }
    TrieOfTrees bind(Tree<BaseKey> tr, Val x) const {
        if (tr.isEmpty()) return TrieOfTrees(x, m_treelist);
        auto [k, left, right] = tr.getNode();
        try {
            auto tt = m_treelist.lookup(k);
            try {
                auto t = tt->lookup(left);
                auto tp = t->bind(right, x);
                auto ttp = tt->bind(tp, left);
                return TrieOfTrees(m_flag, m_treelist.bind(k, ttp));
            } catch (std::runtime_error e2) {
                TrieOfTrees<BaseKey, Val> t = TrieOfTrees<BaseKey, Val>();
                auto tp = t.bind(right, x);
                auto ttp = tt->bind(tp, left);
                return TrieOfTrees(m_flag, m_treelist.bind(k, ttp));
            }
        } catch (std::runtime_error e) {
            TrieOfTrees<BaseKey, TrieOfTrees<BaseKey, Val>> tt = TrieOfTrees<BaseKey, TrieOfTrees<BaseKey, Val>>();
            try {
                auto t = tt.lookup(left);
                auto tp = t->bind(right, x);
                auto ttp = tt.bind(tp, left);
                return TrieOfTrees(m_flag, m_treelist.bind(k, ttp));
            } catch (std::runtime_error e2) {
                TrieOfTrees<BaseKey, Val> t = TrieOfTrees<BaseKey, Val>();
                auto tp = t.bind(right, x);
                auto ttp = tt.bind(tp, left);
                return TrieOfTrees(m_flag, m_treelist.bind(k, ttp));
            }
        }
    }
};
