// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// system
#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

namespace ltb::util {

using NotifyCallback = void (*)();

/// \brief Thread-safe queue implementation.
//         (Feel free to add more functionality)
template <typename T>
class BlockingQueue {
public:
    /// \brief Creates a BlockingQueue and sets the optional `notify_callback`
    /// \param notify_callback - An optional callback that gets called every time an item
    ///                          is added to the queue. This is not necessarily called in
    ///                          the order that items are added to the queue if items are
    ///                          added from multiple threads.
    explicit BlockingQueue(NotifyCallback notify_callback = nullptr);

    auto push_back(T value) -> void;

    template <typename... Args>
    auto emplace_back(Args&&... args) -> void;

    auto pop_front() -> T;

    /// \return the front element or std::nullopt if the timeout is reached.
    auto pop_front(std::chrono::nanoseconds timeout) -> std::optional<T>;

    auto clear() -> void;

    auto size() -> typename std::queue<T>::size_type;
    auto empty() -> bool;

private:
    std::mutex              mutex_;
    std::condition_variable condition_;
    std::queue<T>           queue_;
    NotifyCallback          notify_callback_;
};

template <typename T>
BlockingQueue<T>::BlockingQueue(NotifyCallback notify_callback) : notify_callback_(notify_callback) {}

template <typename T>
auto BlockingQueue<T>::push_back(T value) -> void {
    {
        auto const lock = std::lock_guard(mutex_);
        queue_.push(std::move(value)); // push_back
    }
    condition_.notify_one();
    if (notify_callback_) {
        notify_callback_();
    }
}

template <typename T>
template <typename... Args>
auto BlockingQueue<T>::emplace_back(Args&&... args) -> void {
    {
        auto const lock = std::lock_guard(mutex_);
        queue_.emplace(std::forward<Args>(args)...); // emplace_back
    }
    condition_.notify_one();
    if (notify_callback_) {
        notify_callback_();
    }
}

template <typename T>
auto BlockingQueue<T>::pop_front() -> T {
    std::unique_lock lock(mutex_);
    condition_.wait(lock, [this] { return !queue_.empty(); });
    T rc(std::move(queue_.front()));
    queue_.pop(); // pop_front
    return rc;
}

template <typename T>
auto BlockingQueue<T>::pop_front(std::chrono::nanoseconds timeout) -> std::optional<T> {
    std::unique_lock lock(mutex_);
    if (condition_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
        T rc(std::move(queue_.front()));
        queue_.pop(); // pop_front
        return rc;
    } else {
        return std::nullopt;
    }
}

template <typename T>
auto BlockingQueue<T>::clear() -> void {
    std::queue<T> queue_to_delete{};
    {
        auto const scoped_lock = std::lock_guard(mutex_);
        queue_to_delete.swap(queue_);
    }

    // Delete in the order of arrival
    while (!queue_to_delete.empty()) {
        queue_to_delete.pop();
    }
}

template <typename T>
auto BlockingQueue<T>::size() -> typename std::queue<T>::size_type {
    auto const scoped_lock = std::lock_guard(mutex_);
    return queue_.size();
}

template <typename T>
auto BlockingQueue<T>::empty() -> bool {
    auto const scoped_lock = std::lock_guard(mutex_);
    return queue_.empty();
}

} // namespace ltb::util
