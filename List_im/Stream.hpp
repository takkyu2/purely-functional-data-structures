#ifndef STREAM_HPP
#define STERAM_HPP
#include <string>
#include <memory>
#include <ranges>
#include <iterator>
#include <variant>
#include <functional>
#include <iostream>
#include "../utils/utils.hpp"
#include "../utils/TypeErasure.hpp"

template <typename Elem>
class Stream;

template <typename Elem>
class StreamCell {
public:
    StreamCell() = default;
    StreamCell(Elem v, Stream<Elem> const tail)
        : m_cell(Cell(v,tail)) { }
    explicit StreamCell(Elem v) : m_cell(v) {}
    Elem head() const { return m_cell.value().m_val; }
    Stream<Elem> tail() const { return m_cell.value().m_tail; }
    bool isEmpty() const { return !m_cell; }
private:
    struct Cell {
        Cell(Elem v) : m_val(v) { }
        Cell(Elem v, Stream<Elem> t) : m_val(v), m_tail(t) { }
        Elem m_val;
        Stream<Elem> m_tail;
    };
    std::optional<Cell> m_cell;
};

template <typename Elem>
class Stream {
private:
    std::shared_ptr<FunctionPtr<StreamCell<Elem>()>> m_lazycell;
public:
    Stream() = default;
    Stream(FunctionPtr<StreamCell<Elem>()> f)
        : m_lazycell(std::make_shared<FunctionPtr<StreamCell<Elem>()>>(f)) { }
    Stream(const Stream& stm) = default;
    Stream(Stream && stm) : m_lazycell(std::move(stm.m_lazycell)) { }
    Stream & operator=(Stream && stm) {
        m_lazycell = std::move(stm.m_lazycell);
        return *this;
    }
    StreamCell<Elem> force() const {
        return (*m_lazycell)();
    }
    static Stream empty() {
        return Stream();
    }
    bool isEmpty() const {
        if (!m_lazycell) return true;
        return force().isEmpty();
    }
    Elem head() const {
        return force().head();
    }
    Stream tail() const {
        return force().tail();
    }
    Stream operator+(Stream rhs) const {
        return make_thunk([this_strm=*this, rhs_strm=rhs]() {
                if (this_strm.isEmpty()) return rhs_strm.force();
                return StreamCell<Elem>(this_strm.head(), this_strm.tail() + rhs_strm);
        });
    }
    Stream take(int n) const {
        return make_thunk([this_strm=*this, n=n]() {
                if (n == 0 || this_strm.isEmpty()) return StreamCell<Elem>();
                return StreamCell<Elem>(this_strm.head(), this_strm.tail().take(n-1));
        });
    }
    friend Stream cons(Elem e, const Stream& st) {
        return make_thunk([elem=e, t=st]() {
                return StreamCell<Elem>(elem, t);
        });
    }
    Stream cons(Elem e) const {
        return cons(e, *this);
    }
    Stream drop(int n) const {
        return make_thunk([this_strm=*this, n=n]() {
                return this_strm.drop_helper(n).force();
        });
    }
    Stream drop_helper(int n) const {
        if (isEmpty()) return Stream();
        if (n == 0) return *this;
        return tail().drop(n-1);
    }
    Stream rev() const {
        return reverse();
    }
    Stream reverse() const {
        return make_thunk([this_strm=*this]() {
                    return this_strm.reverse_helper({}).force();
        });
    }
    Stream reverse_helper(Stream r) const {
        if (isEmpty()) return r;
        return tail().reverse_helper(
                make_thunk([head=head(), r=r]() {
                    return StreamCell<Elem>(head, r);
                })
        );
    }
    Stream sort() const {
        return make_thunk([this_strm = *this]() {
                if (this_strm.isEmpty()) return StreamCell<Elem>();
                return this_strm.tail().sort().insert(this_strm.head()).force();
        });
    }
private:
    Stream insert(Elem x) const { // For insertion sort on stream
        return make_thunk([x=x, this_strm=*this]() {
                if (this_strm.isEmpty()) return StreamCell<Elem>(x);
                auto v = this_strm.head(); 
                auto t = this_strm.tail(); 
                if (x < v) return StreamCell<Elem>(x, this_strm); 
                else return StreamCell<Elem>(v, t.insert(x));
        });
    }
    template <typename F>
    static Stream make_thunk(F&& lam) {
        return Stream(make_lazy_val(std::forward<F>(lam)));
    }
};

#endif
