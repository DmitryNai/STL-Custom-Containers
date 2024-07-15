#pragma once

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T, typename Alloc = std::allocator<T>>
class Vector {
public:
    const size_t CAPACITY = 10;

    Vector();

    Vector(size_t count, const T& value);

    Vector(const Vector& other);

    Vector& operator=(const Vector& other);

    Vector(Vector&& other) noexcept;

    Vector& operator=(Vector&& other);

    Vector(std::initializer_list<T> init);

    T& operator[](size_t pos);

    const T& Front() const noexcept;

    T& Back() const noexcept;

    T* Data() const noexcept;

    bool IsEmpty() const noexcept;

    size_t Size() const noexcept;

    size_t Capacity() const noexcept;

    void Reserve(size_t new_cap);

    void Clear() noexcept;

    void Insert(size_t pos, T value);

    void Erase(size_t begin_pos, size_t end_pos);

    void PushBack(T value);

    template <class... Args>
    void EmplaceBack(Args&&... args);

    void PopBack();

    void Resize(size_t count, const T& value);

    ~Vector();

private:
    Alloc allocator_;
    T* data_;
    size_t size_;
    size_t capacity_;
};