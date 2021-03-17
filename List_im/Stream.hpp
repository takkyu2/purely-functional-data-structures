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
    static Stream empty() {
        return Stream();
    }
    bool isEmpty() const {
        return !m_lazycell;
    }
    Elem head() const {
        return (*m_lazycell)().head();
    }
    Stream tail() const {
        return (*m_lazycell)().tail();
    }
    Stream operator+(Stream rhs) const {
        if (isEmpty()) return rhs;
        return Stream(make_lazy_val([this_cell=this->m_lazycell, rhs_cell=rhs]() {
                return StreamCell<Elem>(this_cell->operator()().head(), this_cell->operator()().tail() + rhs_cell);
        }));
    }
    Stream take(int n) const {
        if (n == 0 || isEmpty()) return {};
        return Stream(make_lazy_val([cell=this->m_lazycell, n=n]() {
                return StreamCell<Elem>(cell->operator()().head(), cell->operator()().tail().take(n-1));
        }));
    }
    friend Stream cons(Elem e, const Stream& st) {
        return Stream(make_lazy_val([elem=e, t=st]() {
                return StreamCell<Elem>(elem, t);
        }));
    }
    Stream cons(Elem e) {
        return cons(e, *this);
    }
    Stream drop(int n) const {
        if (isEmpty()) return {};
        if (n == 0) return *this;
        return (*m_lazycell)().tail().drop(n-1);
    }
    Stream reverse() const {
        return reverse_helper({});
    }
    Stream rev() const {
        return reverse();
    }
    Stream reverse_helper(Stream r) const {
        if (isEmpty()) return r;
        return (*m_lazycell)().tail().reverse_helper(
                Stream(make_lazy_val([head=(*m_lazycell)().head(), cell=r]() {
                    return StreamCell<Elem>(head, cell);
            }))
        );
    }
    Stream sort() const {
        if (isEmpty()) return {};
        return (*m_lazycell)().tail().sort().insert((*m_lazycell)().head());
    }
    Stream insert(Elem x) const {
        if (isEmpty()) return Stream(make_lazy_val([x=x](){ return StreamCell<Elem>(x);}));
        return Stream(make_lazy_val([x=x,this_stm = *this]() {
                auto v = (*(this_stm.m_lazycell))().head(); 
                auto t = (*(this_stm.m_lazycell))().tail(); 
                if (x < v) return StreamCell<Elem>(x,this_stm); 
                else return StreamCell<Elem>(v, t.insert(x));
            }));
    }
};

#endif
