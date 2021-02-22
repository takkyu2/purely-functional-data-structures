#ifndef IMLIST_CLASS
#define IMLIST_CLASS
#include <string>
#include <memory>
#include <ranges>

template <typename T>
class ImList {
public:
    ImList() { }
    ImList(std::initializer_list<T> il) {
        auto newone = ImList();
        for (auto elem : il | std::views::reverse)
            newone = newone.cons(elem);
        this->m_head = newone.m_head;
    }
    ImList empty() const { return ImList(); }
    bool isEmpty() const { return m_head == nullptr; }
    ImList cons(T newval) const {
        if (isEmpty()) {
            return ImList(std::make_shared<Node>(Node(newval)));
        } else {
            return ImList(std::make_shared<Node>(Node(newval, this->first())));
        }
    }
    // return head, raise error if empty
    T head() const { return this->first_if("head")->value; }
    // return new ImList, raise error if empty
    ImList tail() const { return ImList(this->first_if("tail")->tail); }
    // concatenate two ImLists
    ImList operator+(ImList rhs) const { return this->isEmpty() ? rhs : (this->tail() + rhs).cons(this->head()); }
    ImList update(int i, T val) const {
        if (i < 0) 
            throw "Out of range: update";
        else if (i == 0)
            return tail().cons(val);
        else
            return this->tail().update(i-1, val).cons(this->head());
    }
private:
    // Internal Node structure
    struct Node {
        Node(T val): value(val) { }
        Node(T val, std::shared_ptr<Node> next): value(val), tail(next) { }
        // Avoid recursion
        ~Node() {
            auto next_node = std::move(tail);
            while (next_node) {
                if (!next_node.unique()) break;
                std::shared_ptr<Node> tail;
                swap(tail, next_node->tail);
                next_node.reset();
                next_node = std::move(tail);
            }
        }
        T value;
        std::shared_ptr<Node> tail;
    };
    // Sentinel for the head
    std::shared_ptr<Node> m_head;
    ImList(std::shared_ptr<Node> head) : m_head(head) { }
    // Get the pointer to the head
    std::shared_ptr<Node> first() const { return m_head; }
    // Get the pointer to the head, raise error if empty
    std::shared_ptr<Node> first_if(std::string em) const { 
        if (isEmpty())
            throw "Error in range: " + em;
        else
            return m_head;
    }
};

#endif
