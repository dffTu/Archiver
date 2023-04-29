#pragma once

#include <memory>

template <typename T>
class PriorityQueue {
private:
    T* array_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void IncreaseCapacity() {
        if (capacity_ == 0) {
            capacity_ = 1;
            array_ = new T[capacity_];
        } else {
            capacity_ *= 2;
            T* ptr = new T[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                ptr[i] = array_[i];
            }
            std::swap(ptr, array_);
            delete[] ptr;
        }
    }

    void SiftDown(size_t index) {
        while (index * 2 + 1 < size_) {
            size_t left = index * 2 + 1;
            size_t right = index * 2 + 2;
            size_t new_index = left;
            if (right < size_ && array_[right] < array_[left]) {
                new_index = right;
            }
            if (array_[index] <= array_[new_index]) {
                break;
            }
            T temp = array_[new_index];
            array_[new_index] = array_[index];
            array_[index] = temp;
            index = new_index;
        }
    }

    void SiftUp(size_t index) {
        while (index != 0 && array_[index] < array_[(index - 1) / 2]) {
            T temp = array_[(index - 1) / 2];
            array_[(index - 1) / 2] = array_[index];
            array_[index] = temp;
            index = (index - 1) / 2;
        }
    }

public:
    PriorityQueue() = default;

    PriorityQueue(const PriorityQueue& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        array_ = other.array_;
    }

    PriorityQueue(PriorityQueue&& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        array_ = other.array_;
        delete[] other.array_;
    }

    void Push(T value) {
        if (capacity_ < size_ + 1) {
            IncreaseCapacity();
        }
        ++size_;
        array_[size_ - 1] = value;
        SiftUp(size_ - 1);
    }

    T Top() const {
        return array_[0];
    }

    void Pop() {
        if (size_ == 0) {
            throw std::runtime_error("Trying to pop elements from empty priority queue");
        }
        array_[0] = array_[size_ - 1];
        --size_;
        SiftDown(0);
    }

    size_t Size() const {
        return size_;
    }

    PriorityQueue& operator=(const PriorityQueue& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        array_ = other.array_;
        return *this;
    }

    PriorityQueue& operator=(PriorityQueue&& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        array_ = other.array_;
        delete[] other.array_;
        return *this;
    }

    ~PriorityQueue() {
        delete[] array_;
    }
};