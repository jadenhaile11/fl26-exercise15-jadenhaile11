// thread_safe_vector.hpp: starter code -- please refer to in-class example for ThreadSafeQueue

#ifndef THREAD_SAFE_VECTOR_HPP
#define THREAD_SAFE_VECTOR_HPP

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <utility>
#include <vector>

template <typename T>
class ThreadSafeVector {
public:
    ThreadSafeVector() = default;
    ThreadSafeVector(const ThreadSafeVector&) = delete;
    ThreadSafeVector& operator=(const ThreadSafeVector&) = delete;

    void push_back(const T& value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            data_.push_back(value);
        }
        condition_.notify_one();
    }

    void push_back(T&& value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            data_.push_back(std::move(value));
        }
        condition_.notify_one();
    }

    // Waits until the vector has an element to remove.
    void wait_and_pop_back(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !data_.empty(); });
        value = std::move(data_.back());
        data_.pop_back();
    }

    // Tries to remove an element without waiting.
    bool try_pop_back(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (data_.empty()) {
            return false;
        }
        value = std::move(data_.back());
        data_.pop_back();
        return true;
    }

    // Checks whether the vector is empty.
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.empty();
    }

    // Returns the current number of elements.
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return data_.size();
    }

private:
    std::vector<T> data_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
};

#endif