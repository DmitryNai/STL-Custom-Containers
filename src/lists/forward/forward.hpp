#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class ForwardList {
private:
    struct Node {
        friend class ForwardListIterator;
        friend class ForwardList;

        explicit Node(const T& val) : data_(val), next_(nullptr) {
        }

    private:
        T data_;
        Node* next_;
    };

public:
    class ForwardListIterator {
    public:
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::forward_iterator_tag;

        explicit ForwardListIterator(Node* node) : current_(node) {
        }

        inline bool operator==(const ForwardListIterator& other) const {
            return current_ == other.current_;
        }

        inline bool operator!=(const ForwardListIterator& other) const {
            return !(*this == other);
        }

        inline reference_type operator*() const {
            return current_->data_;
        }

        ForwardListIterator& operator++() {
            current_ = current_->next_;
            return *this;
        }

        ForwardListIterator operator++(int) {
            ForwardListIterator temp = *this;
            ++(*this);
            return temp;
        }

        inline pointer_type operator->() const {
            return &(current_->data_);
        }

    private:
        Node* current_;

        friend class ForwardList;
    };

public:
    ForwardList() : head_(nullptr), size_(0) {
    }

    explicit ForwardList(size_t sz) : ForwardList() {
        Node* current = nullptr;
        for (size_t i = 0; i < sz; ++i) {
            Node* new_node = new Node(T());
            if (!head_) {
                head_ = new_node;
            } else {
                current->next_ = new_node;
            }
            current = new_node;
            ++size_;
        }
    }

    ForwardList(const std::initializer_list<T>& other) : ForwardList() {
        Node* current = nullptr;
        for (const T& val : other) {
            Node* new_node = new Node(val);
            if (!head_) {
                head_ = new_node;
            } else {
                current->next_ = new_node;
            }
            current = new_node;
            ++size_;
        }
    }

    ForwardList(const ForwardList& other) : ForwardList() {
        Node* current_other = other.head_;
        Node* current = nullptr;
        while (current_other) {
            Node* new_node = new Node(current_other->data_);
            if (!head_) {
                head_ = new_node;
            } else {
                current->next_ = new_node;
            }
            current = new_node;
            current_other = current_other->next_;
            ++size_;
        }
    }

    ForwardList& operator=(const ForwardList& other) {
        if (this != &other) {
            Clear();
            Node* temp = other.head_;
            while (temp) {
                this->PushFront(temp->data_);
                temp = temp->next_;
            }
        }
        return *this;
    }

    ForwardListIterator Begin() const noexcept {
        return ForwardListIterator(head_);
    }

    ForwardListIterator End() const noexcept {
        return ForwardListIterator(nullptr);
    }

    inline T& Front() const {
        return head_->data_;
    }

    inline bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    inline size_t Size() const noexcept {
        return size_;
    }

    void Swap(ForwardList& other) {
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
    }

    void EraseAfter(ForwardListIterator pos) {
        if (!pos.current_ || !pos.current_->next_) {
            return;
        }
        Node* temp = pos.current_->next_;
        pos.current_->next_ = temp->next_;
        delete temp;
        --size_;
    }

    void InsertAfter(ForwardListIterator pos, const T& value) {
        Node* new_node = new Node(value);
        new_node->next_ = pos.current_->next_;
        pos.current_->next_ = new_node;
        ++size_;
    }

    ForwardListIterator Find(const T& value) const {
        Node* temp = head_;
        while (temp) {
            if (temp->data_ == value) {
                return ForwardListIterator(temp);
            }
            temp = temp->next_;
        }
        return End();
    }

    void Clear() noexcept {
        Node* temp = nullptr;
        while (head_) {
            temp = head_;
            head_ = head_->next_;
            delete temp;
        }
        size_ = 0;
    }

    void PushFront(const T& value) {
        Node* new_node = new Node(value);
        new_node->next_ = head_;
        head_ = new_node;
        ++size_;
    }

    void PopFront() {
        if (!head_) {
            throw ListIsEmptyException("Error");
        }
        Node* temp = head_;
        head_ = head_->next_;
        delete temp;
        --size_;
    }

    ~ForwardList() {
        Clear();
    }

private:
    Node* head_;
    size_t size_;
};

namespace std {
template <typename T>
// NOLINTNEXTLINE
void swap(ForwardList<T>& a, ForwardList<T>& b) {
    a.Swap(b);
}
}  // namespace std