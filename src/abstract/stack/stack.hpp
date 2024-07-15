#include <deque>
#include <iostream>
#include <stdexcept>

template <typename T, typename Container = std::deque<T>>
class Stack {
public:
    Stack() = default;

    Stack(const Stack& other) : container_(other.container_) {
    }

    Stack(Stack&& other) noexcept : container_(std::move(other.container_)) {
    }

    explicit Stack(const Container& cont) : container_(cont) {
    }

    ~Stack() = default;

    Stack& operator=(const Stack& other) {
        if (this != &other) {
            container_ = other.container_;
        }
        return *this;
    }

    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            container_ = std::move(other.container_);
        }
        return *this;
    }
    void Push(const T& value) {
        container_.push_back(value);
    }

    void Pop() {
        if (container_.empty()) {
            throw std::out_of_range("Stack is empty");
        }
        container_.pop_back();
    }

    T& Top() {
        if (container_.empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return container_.back();
    }

    const T& Top() const {
        if (container_.empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return container_.back();
    }

    bool Empty() const {
        return container_.empty();
    }

    size_t Size() const {
        return container_.size();
    }

private:
    Container container_;
};