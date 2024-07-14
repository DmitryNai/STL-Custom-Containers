#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T, typename Alloc = std::allocator<T>>
class Deque {
private:
    static const size_t CHUNK_SIZE = 4;

    using AllocatorTraits = std::allocator_traits<Alloc>;
    using PointerAllocator = typename AllocatorTraits::template rebind_alloc<T*>;
    using PointerAllocatorTraits = std::allocator_traits<PointerAllocator>;

    void AllocateNewChunkFront() {
        T** new_chunks = PointerAllocatorTraits::allocate(pointer_allocator_, chunk_count_ + 1);
        T* new_chunk = AllocatorTraits::allocate(allocator_, CHUNK_SIZE);
        std::uninitialized_copy(chunks_, chunks_ + chunk_count_, new_chunks + 1);
        PointerAllocatorTraits::construct(pointer_allocator_, new_chunks, new_chunk);
        PointerAllocatorTraits::deallocate(pointer_allocator_, chunks_, chunk_count_);
        chunks_ = new_chunks;
        ++chunk_count_;
        start_ += CHUNK_SIZE;
    }

    void AllocateNewChunkBack() {
        T** new_chunks = PointerAllocatorTraits::allocate(pointer_allocator_, chunk_count_ + 1);
        T* new_chunk = AllocatorTraits::allocate(allocator_, CHUNK_SIZE);
        std::uninitialized_copy(chunks_, chunks_ + chunk_count_, new_chunks);
        PointerAllocatorTraits::construct(pointer_allocator_, new_chunks + chunk_count_, new_chunk);
        PointerAllocatorTraits::deallocate(pointer_allocator_, chunks_, chunk_count_);
        chunks_ = new_chunks;
        ++chunk_count_;
    }

    T& ChunkElement(int index) const {
        int chunk_index = static_cast<int>(index / CHUNK_SIZE);
        int element_index = static_cast<int>(index % CHUNK_SIZE);
        return chunks_[chunk_index][element_index];
    }

    void DeallocateChunk(T* chunk) {
        AllocatorTraits::deallocate(allocator_, chunk, CHUNK_SIZE);
    }

public:
    Deque()
        : chunks_(nullptr),
          chunk_count_(0),
          start_(0),
          size_(0),
          allocator_(Alloc()),
          pointer_allocator_(PointerAllocator()) {
        AllocateNewChunkBack();
    }

    ~Deque() {
        for (int i = 0; i < chunk_count_; ++i) {
            for (size_t j = 0; j < CHUNK_SIZE; ++j) {
                AllocatorTraits::destroy(allocator_, &chunks_[i][j]);
            }
            DeallocateChunk(chunks_[i]);
        }
        PointerAllocatorTraits::deallocate(pointer_allocator_, chunks_, chunk_count_);
    }

    explicit Deque(size_t count)
        : chunks_(nullptr),
          chunk_count_(0),
          start_(0),
          size_(0),
          allocator_(Alloc()),
          pointer_allocator_(PointerAllocator()) {
        size_t chunks_needed = (count + CHUNK_SIZE - 1) / CHUNK_SIZE;
        for (size_t i = 0; i < chunks_needed; ++i) {
            AllocateNewChunkBack();
        }
    }

    Deque(size_t count, const T& value)
        : chunks_(nullptr),
          chunk_count_(0),
          start_(0),
          size_(0),
          allocator_(Alloc()),
          pointer_allocator_(PointerAllocator()) {
        size_t chunks_needed = (count + CHUNK_SIZE - 1) / CHUNK_SIZE;
        for (size_t i = 0; i < chunks_needed; ++i) {
            AllocateNewChunkBack();
        }
        for (size_t i = 0; i < count; ++i) {
            PushBack(value);
        }
    }

    Deque(std::initializer_list<T> init_list)
        : chunks_(nullptr),
          chunk_count_(0),
          start_(0),
          size_(0),
          allocator_(Alloc()),
          pointer_allocator_(PointerAllocator()) {
        size_t count = init_list.size();
        size_t chunks_needed = (count + CHUNK_SIZE - 1) / CHUNK_SIZE;
        for (size_t i = 0; i < chunks_needed; ++i) {
            AllocateNewChunkBack();
        }
        for (const T& value : init_list) {
            PushBack(value);
        }
    }

    Deque(const Deque& other)
        : chunk_count_(other.chunk_count_),
          start_(other.start_),
          size_(other.size_),
          allocator_(AllocatorTraits::select_on_container_copy_construction(other.allocator_)),
          pointer_allocator_(PointerAllocatorTraits::select_on_container_copy_construction(other.pointer_allocator_)) {
        chunks_ = PointerAllocatorTraits::allocate(pointer_allocator_, chunk_count_);
        for (int i = 0; i < chunk_count_; ++i) {
            chunks_[i] = AllocatorTraits::allocate(allocator_, CHUNK_SIZE);
            std::uninitialized_copy(other.chunks_[i], other.chunks_[i] + CHUNK_SIZE, chunks_[i]);
        }
    }

    Deque& operator=(const Deque& other) {
        if (this != &other) {
            for (int i = 0; i < chunk_count_; ++i) {
                for (size_t j = 0; j < CHUNK_SIZE; ++j) {
                    AllocatorTraits::destroy(allocator_, &chunks_[i][j]);
                }
                DeallocateChunk(chunks_[i]);
            }
            PointerAllocatorTraits::deallocate(pointer_allocator_, chunks_, chunk_count_);

            chunk_count_ = other.chunk_count_;
            start_ = other.start_;
            size_ = other.size_;
            allocator_ = AllocatorTraits::select_on_container_copy_construction(other.allocator_);
            pointer_allocator_ =
                PointerAllocatorTraits::select_on_container_copy_construction(other.pointer_allocator_);
            chunks_ = PointerAllocatorTraits::allocate(pointer_allocator_, chunk_count_);
            for (int i = 0; i < chunk_count_; ++i) {
                chunks_[i] = AllocatorTraits::allocate(allocator_, CHUNK_SIZE);
                std::uninitialized_copy(other.chunks_[i], other.chunks_[i] + CHUNK_SIZE, chunks_[i]);
            }
        }
        return *this;
    }

    Deque(Deque&& other) noexcept
        : chunks_(other.chunks_),
          chunk_count_(other.chunk_count_),
          start_(other.start_),
          size_(other.size_),
          allocator_(std::move(other.allocator_)),
          pointer_allocator_(std::move(other.pointer_allocator_)) {
        other.chunks_ = nullptr;
        other.chunk_count_ = 0;
        other.start_ = 0;
        other.size_ = 0;
    }

    Deque& operator=(Deque&& other) noexcept {
        if (this != &other) {
            for (int i = 0; i < chunk_count_; ++i) {
                for (size_t j = 0; j < CHUNK_SIZE; ++j) {
                    AllocatorTraits::destroy(allocator_, &chunks_[i][j]);
                }
                DeallocateChunk(chunks_[i]);
            }
            PointerAllocatorTraits::deallocate(pointer_allocator_, chunks_, chunk_count_);

            chunks_ = other.chunks_;
            chunk_count_ = other.chunk_count_;
            start_ = other.start_;
            size_ = other.size_;
            allocator_ = std::move(other.allocator_);
            pointer_allocator_ = std::move(other.pointer_allocator_);

            other.chunks_ = nullptr;
            other.chunk_count_ = 0;
            other.start_ = 0;
            other.size_ = 0;
        }
        return *this;
    }

    void PushBack(const T& value) {
        if ((start_ + size_) % CHUNK_SIZE == 0 && size_ != 0) {
            AllocateNewChunkBack();
        }
        AllocatorTraits::construct(allocator_, &ChunkElement(start_ + size_), value);
        ++size_;
    }

    void PushFront(const T& value) {
        if (start_ % CHUNK_SIZE == 0 && size_ != 0) {
            AllocateNewChunkFront();
        }
        AllocatorTraits::construct(allocator_, &ChunkElement(--start_), value);
        ++size_;
    }

    void PopBack() {
        if (size_ == 0) {
            throw std::out_of_range("Deque is empty");
        }
        AllocatorTraits::destroy(allocator_, &ChunkElement(start_ + size_ - 1));
        --size_;
        if ((start_ + size_) % CHUNK_SIZE == 0 && size_ != 0) {
            DeallocateChunk(chunks_[chunk_count_ - 1]);
            --chunk_count_;
        }
    }

    void PopFront() {
        if (size_ == 0) {
            throw std::out_of_range("Deque is empty");
        }
        AllocatorTraits::destroy(allocator_, &ChunkElement(start_));
        ++start_;
        --size_;
    }

    T& operator[](size_t index) {
        if (index < 0 || index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return ChunkElement(start_ + index);
    }

    const T& operator[](size_t index) const {
        if (index < 0 || index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return ChunkElement(start_ + index);
    }

    size_t Size() const {
        return size_;
    }

    bool Empty() const {
        return size_ == 0;
    }

    void Clear() {
        for (int i = 0; i < chunk_count_; ++i) {
            for (size_t j = 0; j < CHUNK_SIZE; ++j) {
                AllocatorTraits::destroy(allocator_, &chunks_[i][j]);
            }
            DeallocateChunk(chunks_[i]);
        }
        PointerAllocatorTraits::deallocate(pointer_allocator_, chunks_, chunk_count_);
        chunk_count_ = 0;
        size_ = 0;
        start_ = 0;
        chunks_ = nullptr;
        AllocateNewChunkBack();
    }

private:
    T** chunks_;
    int chunk_count_;
    int size_;
    int start_;
    Alloc allocator_;
    PointerAllocator pointer_allocator_;
};