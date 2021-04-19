#ifndef SCHEDULEDBINOMIALHEAP
#define SCHEDULEDBINOMIALHEAP
#include "../List/Stream.hpp"
#include "../List/List.hpp"
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"
#include <variant>
#include <tuple>

template <typename Elem>
Stream<Elem> listToStream(ImList<Elem> lis) {
    if (lis.isEmpty()) return Stream<Elem>();
    else return cons(lis.head(), listToStream(lis.tail()));
}

template <typename Arg, typename Result>
using fn_t = Result(*)(Arg);

template <typename Arg, typename Result>
ImList<Result> map(fn_t<Arg, Result> f, ImList<Arg> lis) {
    if (lis.isEmpty()) return ImList<Result>();
    else return cons(f(lis.head()), map(f, lis.tail()));
}

template <typename Elem>
class ScheduledBinomialHeap {
private:
    class Tree {
    public:
        Tree() = default;
        Tree(Elem e) : elem(e) {}
        Tree(Elem e, ImList<Tree> lis) : elem(e), children(lis) { }
        friend Tree link(const Tree t1, const Tree t2) {
            if (t1.elem <= t2.elem)
                return Tree(t1.elem, cons(t2, t1.children));
            else
                return Tree(t2.elem, cons(t1, t2.children));
        }
        int getelem() const {
            return elem;
        }
        Elem elem;
        ImList<Tree> children;
    };
    struct Digit {
        Digit() = default;
        Digit(Tree t) : digits(t) {}
        static Digit makeOne(const Tree t) {
            return Digit(t);
        }
        bool isZero() const {
            return std::holds_alternative<Empty>(digits);
        }
        bool isOne() const {
            return std::holds_alternative<Tree>(digits);
        }
        Tree getOne() const {
            return std::get<Tree>(digits);
        }
        std::variant<Empty, Tree> digits;
    };
    using Schedule = ImList<Stream<Digit>>;
    Stream<Digit> digits;
    Schedule schedule;

    friend Stream<Digit> insTree(const Tree t, const Stream<Digit> ds) {
        if (ds.isEmpty()) return cons(t, ds);
        else if (ds.head().isZero()) return cons(t, ds.tail());
        else return cons(Digit(), insTree(link(t, ds.head().getOne()), ds.tail()));
    }
    
public:
    ScheduledBinomialHeap() = default;
    ScheduledBinomialHeap(Stream<Digit> ds, Schedule sch)
        : digits(ds), schedule(sch) {}
    static ScheduledBinomialHeap empty() {
        return ScheduledBinomialHeap();
    }
    bool isEmpty() const {
        return digits.isEmpty();
    }
    friend Stream<Digit> mrg(const Stream<Digit> ds1,const Stream<Digit> ds2) {
        if (ds1.isEmpty()) return ds2;
        else if (ds2.isEmpty()) return ds1;
        else if (ds1.head().isZero()) return cons(ds2.head(), mrg(ds1.tail(), ds2.tail()));
        else if (ds2.head().isZero()) return cons(ds1.head(), mrg(ds1.tail(), ds2.tail()));
        else return cons(Digit(), insTree(link(ds1.head().getOne(), ds2.head().getOne()), mrg(ds1.tail(), ds2.tail())));
    }
    friend Stream<Digit> mrgWithList(const ImList<Tree> lis,const Stream<Digit> ds) {
        if (lis.isEmpty()) return ds;
        else if (ds.isEmpty()) return listToStream(map(+[](Tree t) {return Digit(t);}, lis)); //unary + for converting lambda into function pointer
        else if (ds.head().isZero()) return cons(lis.head(), mrgWithList(lis.tail(), ds.tail()));
        else return cons(Digit(), insTree(link(lis.head(), ds.head().getOne()), mrgWithList(lis.tail(), ds.tail())));
    }
    friend Stream<Digit> normalize(const Stream<Digit> ds) {
        if (ds.isEmpty()) return ds;
        else {
            auto exec_strm = normalize(ds.tail());
            return ds;
        }
    }
    friend Schedule exec(const Schedule schedule) {
        if (schedule.isEmpty()) return schedule;
        else if (schedule.head().head().isZero())
            return cons(schedule.head().tail(), schedule.tail());
        else return schedule.tail();
    }
    ScheduledBinomialHeap insert(Elem x) const {
        auto newdigits = insTree(Tree(x), digits);
        return ScheduledBinomialHeap(newdigits, exec(exec(cons(newdigits,schedule))));
    }
    friend ScheduledBinomialHeap merge(const ScheduledBinomialHeap h1, const ScheduledBinomialHeap h2) {
        auto newdigits = normalize(mrg(h1.digits, h2.digits));
        return ScheduledBinomialHeap(newdigits, Schedule());
    }
    
    friend std::tuple<Tree, Stream<Digit>> removeMinTree(const Stream<Digit> ds) {
        if (ds.isEmpty()) throw std::runtime_error("empty");
        else if (ds.head().isOne() && ds.tail().isEmpty()) 
            return {ds.head().getOne(), ds.tail()};
        else if (ds.head().isZero()) {
            auto [newtr, newdigits] = removeMinTree(ds.tail());
            return {newtr, cons(Digit(), newdigits)};
        } else {
            auto t = ds.head().getOne();
            auto ds2 = ds.tail();
            auto [t2, ds3] = removeMinTree(ds2);
            if (t.getelem() <= t2.getelem()) return {t, cons(Digit(), ds2)};
            else return {t2, cons(t, ds3)};
        }
    }
    Elem findMin() const {
        auto [t, ds] = removeMinTree(digits);
        return t.getelem();
    }
    ScheduledBinomialHeap deleteMin() {
        auto [t, ds] = removeMinTree(digits);
        auto ds2 = mrg(listToStream(map(+[](Tree t) {return Digit(t);}, t.children.rev())), ds); //unary + for converting lambda into function pointer
        return ScheduledBinomialHeap(normalize(ds2), {});
    }
    ScheduledBinomialHeap fastdeleteMin() {
        auto [t, ds] = removeMinTree(digits);
        auto ds2 = mrgWithList(t.children.rev(), ds);
        return ScheduledBinomialHeap(normalize(ds2), {});
    }
};
#endif
