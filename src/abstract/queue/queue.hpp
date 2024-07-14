#include <deque>
#include <iostream>
#include <stdexcept>

template <typename T, typename Container = std::deque<T>>
class Queue {
public:
    Queue() = default;

    Queue(const Queue& other) : container_(other.container_) {
    }

    Queue(Queue&& other) noexcept : container_(std::move(other.container_)) {
    }

    explicit Queue(const Container& cont) : container_(cont) {
    }

    ~Queue() = default;

    Queue& operator=(const Queue& other) {
        if (this != &other) {
            container_ = other.container_;
        }
        return *this;
    }

    Queue& operator=(Queue&& other) noexcept {
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
            throw std::out_of_range("Queue is empty");
        }
        container_.pop_front();
    }

    T& Front() {
        if (container_.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return container_.front();
    }

    const T& Front() const {
        if (container_.empty()) {
            throw std::out_of_range("Queue is empty");
        }
        return container_.front();
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