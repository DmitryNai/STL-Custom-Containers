#pragma once

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class List {
private:
    struct BaseNode {
        BaseNode* next;
        BaseNode* prev;
        BaseNode() = default;
        BaseNode(BaseNode* next, BaseNode* prev) : next(next), prev(prev) {
        }

        friend class ListIterator;
        friend class List;
    };
    struct Node : public BaseNode {
        T value;
        Node() = default;
        Node(BaseNode* next, BaseNode* prev, const T& value) : BaseNode(next, prev), value(value) {
        }
    };

public:
    template <bool isConst>
    class ListIterator {
    public:
        // NOLINTNEXTLINE
        using baseNode_type = std::conditional_t<isConst, const BaseNode*, BaseNode*>;
        // NOLINTNEXTLINE
        using node_type = std::conditional_t<isConst, const Node*, Node*>;
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = std::conditional_t<isConst, const T&, T&>;
        // NOLINTNEXTLINE
        using pointer_type = std::conditional_t<isConst, const T*, T*>;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::bidirectional_iterator_tag;

        inline bool operator==(const ListIterator& other) const {
            return ptr_ == other.ptr_;
        };

        inline bool operator!=(const ListIterator& other) const {
            return (ptr_ != other.ptr_);
        };

        inline reference_type operator*() const {
            return static_cast<node_type>(ptr_)->value;
        };

        ListIterator& operator++() {
            ptr_ = ptr_->next;
            return *this;
        };

        ListIterator operator++(int) {
            ListIterator temp = *this;
            ++(*this);
            return temp;
        };

        ListIterator& operator--() {
            ptr_ = ptr_->prev;
            return *this;
        };

        ListIterator operator--(int) {
            ListIterator temp = *this;
            --(*this);
            return temp;
        };

        inline pointer_type operator->() const {
            return &(ptr_->value);
        };

    private:
        baseNode_type ptr_;
        ListIterator() = default;
        explicit ListIterator(baseNode_type ptr) : ptr_(ptr) {
        }
        friend class List;
    };

public:
    using ConstListIterator = ListIterator<true>;
    using LstIterator = ListIterator<false>;

public:
    List() : fake_node_(&fake_node_, &fake_node_), sz_(0) {
    }

    explicit List(size_t sz) : List() {
        for (size_t i = 0; i < sz; ++i) {
            this->PushBack(T());
        }
    }

    List(const std::initializer_list<T>& other) : List() {
        auto it = other.begin();
        while (sz_ < other.size()) {
            this->PushBack(*(it++));
        }
    }

    List(const List& other) : List() {
        for (auto it = other.Begin(); it != other.End(); ++it) {
            this->PushBack(*it);
        }
    }

    List& operator=(const List& other) {
        if (this != &other) {
            List temp(other);
            Swap(temp);
        }
        return *this;
    }
    LstIterator Begin() noexcept {
        return LstIterator(fake_node_.next);
    }

    LstIterator End() noexcept {
        return LstIterator(&fake_node_);
    }

    ConstListIterator Begin() const noexcept {
        return ConstListIterator(fake_node_.next);
    }

    ConstListIterator End() const noexcept {
        return ConstListIterator(&fake_node_);
    }

    inline T& Front() const {
        return static_cast<Node*>(fake_node_.next)->value;
    }

    inline T& Back() const {
        return static_cast<Node*>(fake_node_.prev)->value;
    }

    inline bool IsEmpty() const noexcept {
        return sz_ == 0;
    }

    inline size_t Size() const noexcept {
        return sz_;
    }

    void Swap(List& other) {
        std::swap(fake_node_.prev->next, other.fake_node_.prev->next);
        std::swap(fake_node_.next->prev, other.fake_node_.next->prev);
        std::swap(fake_node_.prev, other.fake_node_.prev);
        std::swap(fake_node_.next, other.fake_node_.next);
        std::swap(sz_, other.sz_);
    }

    LstIterator Find(const T& value) {
        for (LstIterator it = Begin(); it != End(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return End();
    }

    void Erase(LstIterator pos) {
        BaseNode* node_to_delete = pos.ptr_;
        node_to_delete->prev->next = node_to_delete->next;
        node_to_delete->next->prev = node_to_delete->prev;
        delete static_cast<Node*>(node_to_delete);
        --sz_;
    }

    void Insert(LstIterator pos, const T& o_value) {
        BaseNode* current_node = pos.ptr_;
        Node* new_node = new Node(current_node, current_node->prev, o_value);
        current_node->prev->next = new_node;
        current_node->prev = new_node;
        ++sz_;
    }

    void Clear() noexcept {
        while (fake_node_.next != &fake_node_) {
            BaseNode* temp = fake_node_.next;
            fake_node_.next = temp->next;
            delete static_cast<Node*>(temp);
        }
        fake_node_.prev = &fake_node_;
        sz_ = 0;
    }

    void PushBack(const T& value) {
        Node* new_node = nullptr;
        new_node = static_cast<Node*>(operator new(sizeof(Node)));
        new (new_node) Node(&fake_node_, fake_node_.prev, value);
        fake_node_.prev->next = new_node;
        fake_node_.prev = new_node;
        ++sz_;
    }

    void PushFront(const T& value) {
        Node* new_node = nullptr;
        new_node = static_cast<Node*>(operator new(sizeof(Node)));
        new (new_node) Node(fake_node_.next, &fake_node_, value);
        fake_node_.next->prev = new_node;
        fake_node_.next = new_node;
        ++sz_;
    }

    void PopBack() {
        if (fake_node_.next == &fake_node_) {
            throw ListIsEmptyException("error");
        }
        BaseNode* last_node = fake_node_.prev;
        fake_node_.prev = last_node->prev;
        last_node->prev->next = &fake_node_;
        delete static_cast<Node*>(last_node);
        --sz_;
    }

    void PopFront() {
        if (fake_node_.prev == &fake_node_) {
            throw ListIsEmptyException("error");
        }
        BaseNode* first_node = fake_node_.next;
        fake_node_.next = first_node->next;
        first_node->next->prev = &fake_node_;
        delete static_cast<Node*>(first_node);
        --sz_;
    }

    ~List() {
        Clear();
    }

private:
    BaseNode fake_node_;
    size_t sz_ = 0;
};

namespace std {
// Global swap overloading
template <typename T>
// NOLINTNEXTLINE
void swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std
