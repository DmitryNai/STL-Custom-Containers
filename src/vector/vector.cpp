#include "vector.hpp"

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector() : data_(nullptr), size_(0), capacity_(0) {
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_t count, const T& value) : Vector() {
    Reserve(count);
    for (size_t i = 0; i < count; ++i) {
        std::allocator_traits<Alloc>::construct(allocator_, data_ + i, value);
    }
    size_ = count;
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector& other) : Vector() {
    Reserve(other.size_);
    for (size_t i = 0; i < other.size_; ++i) {
        std::allocator_traits<Alloc>::construct(allocator_, data_ + i, other.data_[i]);
    }
    size_ = other.size_;
}

template <typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(const Vector& other) {
    if (this != &other) {
        Vector<T> temp(other);
        *this = std::move(temp);
    }
    return *this;
}

template <typename T, typename Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator=(Vector&& other) {
    if (this != &other) {
        Clear();
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    return *this;
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector&& other) noexcept : Vector() {
    *this = std::move(other);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<T> init) : Vector() {
    Reserve(init.size());
    auto it = init.begin();
    for (size_t i = 0; i < init.size(); ++i) {
        std::allocator_traits<Alloc>::construct(allocator_, data_ + i, *(it++));
    }
    size_ = init.size();
}

template <typename T, typename Alloc>
T& Vector<T, Alloc>::operator[](size_t pos) {
    if (pos >= size_) {
        throw std::out_of_range("Vector access out of range");
    }
    return data_[pos];
}

template <typename T, typename Alloc>
bool Vector<T, Alloc>::IsEmpty() const noexcept {
    return size_ == 0;
}

template <typename T, typename Alloc>
const T& Vector<T, Alloc>::Front() const noexcept {
    return data_[0];
}

template <typename T, typename Alloc>
T& Vector<T, Alloc>::Back() const noexcept {
    return data_[size_ - 1];
}

template <typename T, typename Alloc>
T* Vector<T, Alloc>::Data() const noexcept {
    return data_;
}

template <typename T, typename Alloc>
size_t Vector<T, Alloc>::Size() const noexcept {
    return size_;
}

template <typename T, typename Alloc>
size_t Vector<T, Alloc>::Capacity() const noexcept {
    return capacity_;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::Reserve(size_t new_cap) {
    if (new_cap > capacity_ || capacity_ == 0) {
        if (new_cap < CAPACITY) {
            new_cap = CAPACITY;
        }
        T* new_data = allocator_.allocate(new_cap);
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<Alloc>::construct(allocator_, new_data + i, std::move(data_[i]));
            std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
        }
        if (data_) {
            allocator_.deallocate(data_, capacity_);
        }
        data_ = new_data;
        capacity_ = new_cap;
    }
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::Clear() noexcept {
    for (size_t i = 0; i < size_; ++i) {
        std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
    }
    if (data_) {
        allocator_.deallocate(data_, capacity_);
    }
    data_ = nullptr;
    size_ = 0;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::Insert(size_t pos, T value) {
    if (size_ >= capacity_) {
        Reserve((capacity_ == 0) ? 1 : 2 * capacity_);
    }
    if (pos < size_) {
        for (size_t i = size_; i > pos; --i) {
            std::allocator_traits<Alloc>::construct(allocator_, data_ + i, std::move(data_[i - 1]));
            std::allocator_traits<Alloc>::destroy(allocator_, data_ + i - 1);
        }
    }
    std::allocator_traits<Alloc>::construct(allocator_, data_ + pos, std::move(value));
    ++size_;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::Erase(size_t begin_pos, size_t end_pos) {
    if (begin_pos >= size_ || begin_pos >= end_pos) {
        return;
    }
    end_pos = std::min(end_pos, size_);
    size_t num_to_remove = end_pos - begin_pos;
    for (size_t i = begin_pos; i < size_ - num_to_remove; ++i) {
        std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
        std::allocator_traits<Alloc>::construct(allocator_, data_ + i, std::move(data_[i + num_to_remove]));
    }
    for (size_t i = size_ - num_to_remove; i < size_; ++i) {
        std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
    }
    size_ -= num_to_remove;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::PushBack(T value) {
    Insert(size_, std::move(value));
}

template <typename T, typename Alloc>
template <class... Args>
void Vector<T, Alloc>::EmplaceBack(Args&&... args) {
    Reserve(size_ + 1);
    std::allocator_traits<Alloc>::construct(allocator_, data_ + size_, std::forward<Args>(args)...);
    ++size_;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::PopBack() {
    if (size_ > 0) {
        std::allocator_traits<Alloc>::destroy(allocator_, data_ + --size_);
    }
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::Resize(size_t count, const T& value) {
    if (count < size_) {
        for (size_t i = count; i < size_; ++i) {
            std::allocator_traits<Alloc>::destroy(allocator_, data_ + i);
        }
        size_ = count;
    } else if (count > size_) {
        Reserve(count);
        for (size_t i = size_; i < count; ++i) {
            std::allocator_traits<Alloc>::construct(allocator_, data_ + i, value);
        }
        size_ = count;
    }
}

template <typename T, typename Alloc>
Vector<T, Alloc>::~Vector() {
    Clear();
}

template <>
class Vector<void*> {
public:
    Vector() : data_(nullptr), size_(0), capacity_(0) {
    }

    Vector& operator=(const Vector& other) {
        Reserve(other.size_);
        Clear();
        for (size_t index = 0; index < other.size_; ++index) {
            data_[index] = other.data_[index];
        }
        return *this;
    }

    Vector& operator=(Vector&& other) {
        Clear();
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        return *this;
    }

    size_t Size() const noexcept {
        return size_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void* Front() const {
        return data_[0];
    }

    void* Back() const {
        return data_[size_ - 1];
    }

    void Reserve(size_t new_size) {
        if (new_size <= capacity_) {
            return;
        }
        void** new_data = static_cast<void**>(malloc(new_size * sizeof(void*)));
        for (size_t index = 0; index < size_; ++index) {
            new (new_data + index) void*(data_[index]);
        }
        free(data_);
        capacity_ = new_size;
        data_ = new_data;
    }

    void PushBack(void* value) {
        if (size_ == capacity_) {
            Reserve((capacity_ == 0) ? 1 : 2 * capacity_);
        }
        data_[size_] = value;
        ++size_;
    }

    void Clear() noexcept {
        for (size_t index = 0; index < size_; ++index) {
            if (data_[index]) {
                free(data_[index]);
            }
        }
        size_ = 0;
    }

    ~Vector() noexcept {
        Clear();
        free(data_);
    }

private:
    void** data_;
    size_t size_;
    size_t capacity_;
};