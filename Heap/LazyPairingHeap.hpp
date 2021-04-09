#include <variant>
#include <tuple>
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"
#include "../List/List.hpp"

template <Ordered Elem>
class Heap {
private:
    using Suspheap = FunctionPtr<Heap()>;
    struct Tree {
        Tree() = default;
        Tree(Elem e) 
            : elem(e), 
            m_odd_field(std::make_shared<Heap>(Heap())),
            m_susp_heap(std::make_shared<Suspheap>(make_thunk([](){return Heap();}))) { }
        Tree(Elem e, std::shared_ptr<Heap> lis, std::shared_ptr<Suspheap> lis2) 
            : elem(e), m_odd_field(lis), m_susp_heap(lis2) { }
        Elem getelem() const {
            return elem;
        }
        Elem elem;
        std::shared_ptr<Heap> m_odd_field;
        std::shared_ptr<Suspheap> m_susp_heap;
    };
    std::tuple<Elem, std::shared_ptr<Heap>, std::shared_ptr<Suspheap>> get() const {
        auto t = std::get<Tree>(m_heap);
        return {t.elem, t.m_odd_field, t.m_susp_heap};
    }
    template <typename F>
    static Suspheap make_thunk(F&& lam) {
        return make_lazy_val(std::forward<F>(lam));
    }
    static Heap link(const Heap h1, const Heap h2) {
        Tree t1 = std::get<Tree>(h1.m_heap), t2 = std::get<Tree>(h2.m_heap);
        if (t1.m_odd_field->isEmpty()) return Tree(t1.elem, std::make_shared<Heap>(h2), t1.m_susp_heap); 
        else return Tree(t1.elem, std::make_shared<Heap>(Heap()), std::make_shared<Suspheap>(make_thunk([h2=h2,t1=t1](){
                    return merge(merge(h2, (*t1.m_odd_field)), (*(t1.m_susp_heap))());
                })));
    }
    std::variant<Empty, Tree> m_heap;
public:
    Heap() = default;
    Heap(Elem e) : m_heap(Tree(e)) { }
    Heap(Tree t) : m_heap(t) { }
    Heap(Elem e, ImList<std::shared_ptr<Heap>> children) : m_heap(Tree(e, children)) { }
    bool isEmpty() const {
        return std::holds_alternative<Empty>(m_heap);
    }
    Elem findMin() const {
        if (isEmpty()) throw std::runtime_error("error in findMin, empty");
        return std::get<Tree>(m_heap).getelem();
    }
    friend Heap merge(const Heap h1, const Heap h2) {
        if (h1.isEmpty()) return h2;
        if (h2.isEmpty()) return h1;
        auto [x, h1o, h1h] = h1.get();
        auto [y, h2o, h2h] = h2.get();
        if (x <= y)
            return link(h1, h2);
        else
            return link(h2, h1);
    }
    Heap insert(Elem e) const {
        return merge(Heap(e), *this);
    }
    friend Heap mergePairs(ImList<std::shared_ptr<Heap>> hc) {
        if (hc.isEmpty()) return Heap();
        if (hc.tail().isEmpty()) return *hc.head();
        auto h1 = hc.head(), h2 = hc.tail().head(), hs = hc.tail().tail();
        return merge(merge(*h1, *h2), mergePairs(hs));
    }
    Heap deleteMin() const {
        if (isEmpty()) return Heap();
        auto [x, ho, hs] = get();
        return merge(*ho, (*hs)());
    }
};
