#include <iostream>
#include <concepts>
#include <variant>
#include <memory>
#include <functional>
#include <stack>
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"
#include "../List/List.hpp"

template <typename Elem>
ImList<Elem> mrg(const ImList<Elem> xs, const ImList<Elem> ys) {
    if (xs.isEmpty()) return ys;
    if (ys.isEmpty()) return xs;
    if (xs.head() <= ys.head()) return cons(xs.head(), mrg(xs.tail(), ys));
    else return cons(ys.head(), mrg(xs, ys.tail()));
}
template <typename Elem>
ImList<ImList<Elem>> addSeg(const ImList<Elem> seg, const ImList<ImList<Elem>> segs, const int sz) {
    if (sz % 2 == 0) return cons(seg, segs);
    else return addSeg(mrg(seg, segs.head()), segs.tail(), sz / 2);
}
template <typename Elem>
ImList<Elem> mrgAll(const ImList<Elem> xs, const ImList<ImList<Elem>> segs) {
    if (segs.isEmpty()) return xs;
    return mrgAll(mrg(xs, segs.head()), segs.tail());
}

template <typename Elem>
class BottomUpMergeSort {
public:
    using suspll = FunctionPtr<ImList<ImList<Elem>>()>;
    BottomUpMergeSort() {
        *this = empty();
    }
    BottomUpMergeSort(int sz, suspll mglist): sz(sz), mglist(mglist) {}
    static BottomUpMergeSort empty() {
        return {0, [](){return ImList<ImList<Elem>>::empty();}};
    }
    BottomUpMergeSort add(Elem x) const {
        return BottomUpMergeSort(sz+1, [this_ms=*this, x](){
            return addSeg(cons(x, ImList<Elem>::empty()), this_ms.force(), this_ms.sz);
        });
    }
    ImList<Elem> sort() const {
        return mrgAll(ImList<Elem>::empty(), force());
    }
private:
    ImList<ImList<Elem>> force() const {
        return mglist();
    }
    int sz;
    suspll mglist;
};

