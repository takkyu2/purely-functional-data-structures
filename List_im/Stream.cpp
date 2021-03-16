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
        return Stream([this_cell=this->m_lazycell, rhs_cell=rhs]() {
                return StreamCell<Elem>(this_cell->operator()().head(), this_cell->operator()().tail() + rhs_cell);
        });
    }
    Stream take(int n) const {
        if (n == 0 || isEmpty()) return {};
        return Stream([cell=this->m_lazycell, n=n]() {
                return StreamCell<Elem>(cell->operator()().head(), cell->operator()().tail().take(n-1));
        });
    }
    friend Stream cons(Elem e, const Stream& st) {
        return Stream([elem=e, t=st]() {
                return StreamCell<Elem>(elem, t);
        });
    }
    Stream drop(int n) const {
        if (isEmpty()) return {};
        if (n == 0) return *this;
        return (*m_lazycell)().tail().drop(n-1);
    }
    Stream reverse() {
        return reverse_helper({});
    }
    Stream reverse_helper(Stream r) {
        if (isEmpty()) return r;
        return (*m_lazycell)().tail().reverse_helper(
                Stream([head=(*m_lazycell)().head(), cell=r]() {
                    return StreamCell<Elem>(head, cell);
            })
        );
    }
    Stream sort() {
        if (isEmpty()) return {};
        return (*m_lazycell)().tail().sort().insert((*m_lazycell)().head());
    }
    Stream insert(Elem x) {
        if (isEmpty()) return Stream([x=x](){ return StreamCell<Elem>(x);});
        return Stream([x=x,this_stm = *this]() {
                auto v = (*(this_stm.m_lazycell))().head(); 
                auto t = (*(this_stm.m_lazycell))().tail(); 
                if (x < v) return StreamCell<Elem>(x,this_stm); 
                else return StreamCell<Elem>(v, t.insert(x));
            });
    }
};


int main() {
    Stream<int> st, st2;
    for (int i = 0; i < 10; ++i) {
        st = cons(i, st);
    }
    for (int i = 10; i < 20; ++i) {
        st2 = cons(i, st2);
    }
    Stream<int> st3 = st + st2;
    Stream<int> st4 = st3.drop(3);
    Stream<int> st5 = st3.reverse();

    Stream<int> st2v = st3.sort();

    std::cout << std::endl;
    for (int i = 0; i < 20; ++i) {
        std::cout << st3.head() << std::endl;
        st3 = st3.tail();
    }
    std::cout << std::endl;
    for (int i = 0; i < 20; ++i) {
        std::cout << st2v.head() << std::endl;
        st2v = st2v.tail();
    }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << st4.head() << std::endl;
        st4 = st4.tail();
    }
    std::cout << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::cout << st5.head() << std::endl;
        st5 = st5.tail();
    }
}
