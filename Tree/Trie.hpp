#ifndef TRIE
#define TRIE
#include "BinarySearchTree.hpp"
#include "../List/List.hpp"
#include <optional>

template <typename BaseKey, typename Val>
class Trie {
private:
    using Key = ImList<BaseKey>;
    std::optional<Val> m_flag;
    FiniteMap<BaseKey, std::shared_ptr<Trie<BaseKey, Val>>> m_treelist;
public:
    Trie() = default;
    Trie(Val x, FiniteMap<BaseKey, std::shared_ptr<Trie<BaseKey, Val>>> m_treelist )
        : m_flag(x), m_treelist(m_treelist) {}
    Trie(std::optional<Val> x, FiniteMap<BaseKey, std::shared_ptr<Trie<BaseKey, Val>>> m_treelist )
        : m_flag(x), m_treelist(m_treelist) {}
    static Trie empty() {
        return Trie();
    }
    Val lookup(Key key) const {
        if (key.isEmpty()) {
            if (!m_flag.has_value()) {
                throw std::runtime_error("NOTFOUND");
            } else {
                return m_flag.value();
            }
        } else {
            return m_treelist.lookup(key.head())->lookup(key.tail());
        }
    }
    Trie bind(Key key, Val x) const {
        if (key.isEmpty()) return Trie(x, m_treelist);
        else {
            try {
                auto t = m_treelist.lookup(key.head());
                auto tp = t->bind(key.tail(), x);
                return Trie(m_flag, m_treelist.bind(key.head(), std::make_shared<Trie<BaseKey, Val>>(tp)));
            } catch (std::runtime_error e) {
                auto t = empty();
                auto tp = t.bind(key.tail(), x);
                return Trie(m_flag, m_treelist.bind(key.head(), std::make_shared<Trie<BaseKey, Val>>(tp)));
            }
        }
    }
};
#endif
