#include "List.hpp"
#include "Stream.hpp"
#include <tuple>

template <typename Elem>
ImList<Elem> streamToList(Stream<Elem> xs) {
    if (xs.isEmpty()) return ImList<Elem>();
    else return cons(xs.head(), streamToList(xs.tail()));
}

template <typename Arg, typename Result>
using fn_t = Result(*)(Arg);

template <typename Arg, typename Result>
ImList<Result> map(fn_t<Arg, Result> f, ImList<Arg> lis) {
    if (lis.isEmpty()) return ImList<Result>();
    else return cons(f(lis.head()), map(f, lis.tail()));
}

template <typename Elem>
std::tuple<Stream<Elem>, ImList<Stream<Elem>>> exec2(std::tuple<Stream<Elem>> seg);

template <typename Elem>
class ScheduledBottomUpMergeSort {
private:
    using Schedule = ImList<Stream<Elem>>;
    using Segment = std::tuple<Stream<Elem>, Schedule>;
    using Segments = ImList<Segment>;
    struct Sortable {
        int sz;
        Segments segs;
    };
    static Schedule exec1(const Schedule sch) {
        if (sch.isEmpty()) return sch;
        else if (sch.head().isEmpty()) return exec1(sch.tail());
        else return cons(sch.head().tail(), sch.tail());
    }
    static Segment exec2(Segment seg) {
        auto [xs, sched] = seg;
        return {xs, exec1(exec1(sched))};
    }
    Sortable m_sortable;
    static Stream<Elem> mrg(Stream<Elem> xs, Stream<Elem> ys) {
        if (xs.isEmpty()) return ys;
        else if (ys.isEmpty()) return xs;
        else {
            if (xs.head() <= ys.head()) {
                return cons(xs.head(), mrg(xs.tail(), ys));
            } else {
                return cons(ys.head(), mrg(xs, ys.tail()));
            }
        }
    }
    static Stream<Elem> mrgAll(Stream<Elem> xs, Segments ys) {
        if (ys.isEmpty()) return xs;
        else {
            auto [xsp, tmp] = ys.head();
            return mrgAll(mrg(xs,xsp), ys.tail());
        }
    }
public:
    ScheduledBottomUpMergeSort() = default;
    ScheduledBottomUpMergeSort(Sortable s) : m_sortable(s) {}
    static ScheduledBottomUpMergeSort empty() {
        return ScheduledBottomUpMergeSort();
    }
    static Segments addSeg(const Stream<Elem> xs, const Segments segs, const int sz, const Schedule rsched) {
        if (sz % 2 == 0) return cons({xs, rsched.rev()}, segs);
        auto [xsp, tmp] = segs.head();
        auto xspp = mrg(xs, xsp);
        return addSeg(xspp, segs.tail(), sz / 2, cons(xspp, rsched));
    }
    ScheduledBottomUpMergeSort add(Elem x) const {
        auto segsp = addSeg(cons(x, Stream<Elem>()), m_sortable.segs, m_sortable.sz, Schedule());
        return Sortable(m_sortable.sz+1, map(&exec2, segsp));
    }
    ImList<Elem> sort() const {
        return streamToList(mrgAll({}, m_sortable.segs));
    }
};
