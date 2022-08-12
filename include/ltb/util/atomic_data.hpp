// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// standard
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

namespace ltb::util {

struct ScopedLock {
    std::shared_ptr<std::mutex>                  mutex = nullptr;
    std::shared_ptr<std::lock_guard<std::mutex>> lock  = nullptr;
};

/**
 * @brief Owns complex data that can be accessed in a thread safe way.
 *
 * Example:
 *
 *     struct MyComplexData {
 *         int thing1;
 *         bool thing2;
 *         std::vector<double> more_things;
 *         OtherStruct complex_thing;
 *     };
 *
 *     ltb::util::AtomicData<MyComplexData> shared_data;
 *
 *     ... Later, in different threads
 *
 *     shared_data.use_safely([] (MyComplexData& data) {
 *         // Do things with 'data' here:
 *         ...
 *     });
 *
 *     ...
 */
template <typename T>
class AtomicData {
public:
    explicit AtomicData(T&& data = T{});

    template <typename... Args>
    explicit AtomicData(Args&&... args);

    /// \brief Use the data in a thread safe manner.
    template <typename Func>
    auto use_safely(Func func);

    /// \brief Use the data in a thread safe manner.
    template <typename Func>
    auto use_safely(Func func) const;

    /// \brief Safely get a copy of the data (name is taken from std::atomic::load).
    auto load() const -> T;

    /// \brief Safely set the data (name is taken from std::atomic::store).
    auto store(T t) -> void;

    /// \brief Wait for 'notify_one' or 'notify_all' to be called
    ///        before using the data in a thread safe manner.
    /// \param predicate - a predicate that must be true for `func` to be invoked.
    template <typename Pred, typename Func>
    auto wait_to_use_safely(Pred predicate, Func func) -> void;

    /// \brief Wait for 'notify_one' or 'notify_all' to be called
    ///        before using the data in a thread safe manner.
    /// \param predicate - a predicate that must be true for `func` to be invoked.
    template <typename Pred, typename Func>
    auto wait_to_use_safely(Pred predicate, Func func) const -> void;

    /// \brief Wait for 'notify_one' or 'notify_all' to be called
    ///        before using the data in a thread safe manner.
    /// \param duration - the maximum length of time this function will wait before returning.
    /// \param predicate - a predicate that must be true for `func` to be invoked.
    template <typename Rep, typename Period, typename Pred, typename Func>
    auto wait_to_use_safely(std::chrono::duration<Rep, Period> const& duration, Pred predicate, Func func) -> bool;

    /// \brief Wait for 'notify_one' or 'notify_all' to be called
    ///        before using the data in a thread safe manner.
    /// \param duration - the maximum length of time this function will wait before returning.
    /// \param predicate - a predicate that must be true for `func` to be invoked.
    template <typename Rep, typename Period, typename Pred, typename Func>
    auto wait_to_use_safely(std::chrono::duration<Rep, Period> const& duration, Pred predicate, Func func) const
        -> bool;

    /// \brief Allow one 'wait_to_use_safely' function to continue.
    auto notify_one() -> void;

    /// \brief Allow all 'wait_to_use_safely' functions to continue.
    auto notify_all() -> void;

    /// \brief Prevent this data from being accessed until the returned object is destructed.
    [[nodiscard]] auto scoped_lock() const -> ScopedLock;

private:
    std::shared_ptr<std::mutex>              lock_;
    std::shared_ptr<std::condition_variable> condition_;
    T                                        data_;
};

template <typename T>
AtomicData<T>::AtomicData(T&& data)
    : lock_(std::make_shared<std::mutex>()),
      condition_(std::make_shared<std::condition_variable>()),
      data_(std::forward<T>(data)) {}

template <typename T>
template <typename... Args>
AtomicData<T>::AtomicData(Args&&... args)
    : lock_(std::make_shared<std::mutex>()),
      condition_(std::make_shared<std::condition_variable>()),
      data_(std::forward<Args>(args)...) {}

template <typename T>
template <typename Func>
auto AtomicData<T>::use_safely(Func func) {
    std::lock_guard<std::mutex> scoped_lock(*lock_);
    return func(data_);
}

template <typename T>
template <typename Func>
auto AtomicData<T>::use_safely(Func func) const {
    std::lock_guard<std::mutex> scoped_lock(*lock_);
    return func(data_);
}

template <typename T>
auto AtomicData<T>::load() const -> T {
    return use_safely([](auto data) { return data; });
}

template <typename T>
auto AtomicData<T>::store(T data) -> void {
    use_safely([data](auto& stored_data) { stored_data = data; });
}

template <typename T>
template <typename Pred, typename Func>
auto AtomicData<T>::wait_to_use_safely(Pred predicate, Func func) -> void {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    condition_->wait(unlockable_lock, [&] { return predicate(data_); });
    func(data_);
}

template <typename T>
template <typename Pred, typename Func>
auto AtomicData<T>::wait_to_use_safely(Pred predicate, Func func) const -> void {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    condition_->wait(unlockable_lock, [&] { return predicate(data_); });
    func(data_);
}

template <typename T>
template <typename Rep, typename Period, typename Pred, typename Func>
auto AtomicData<T>::wait_to_use_safely(std::chrono::duration<Rep, Period> const& duration, Pred predicate, Func func)
    -> bool {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    if (condition_->wait_for(unlockable_lock, duration, [&] { return predicate(data_); })) {
        func(data_);
        return true;
    }
    return false;
}

template <typename T>
template <typename Rep, typename Period, typename Pred, typename Func>
auto AtomicData<T>::wait_to_use_safely(std::chrono::duration<Rep, Period> const& duration,
                                       Pred                                      predicate,
                                       Func                                      func) const -> bool {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    if (condition_->wait_for(unlockable_lock, duration, [&] { return predicate(data_); })) {
        func(data_);
        return true;
    }
    return false;
}

template <typename T>
auto AtomicData<T>::notify_one() -> void {
    condition_->notify_one();
}

template <typename T>
auto AtomicData<T>::notify_all() -> void {
    condition_->notify_all();
}

template <typename T>
auto AtomicData<T>::scoped_lock() const -> ScopedLock {
    ScopedLock lock{lock_};
    lock.lock = std::make_shared<std::lock_guard<std::mutex>>(*lock.mutex);
    return lock;
}

} // namespace ltb::util
