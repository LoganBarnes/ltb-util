// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "blocking_queue.hpp"
#include "result.hpp"

// standard
#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace ltb::util {

/// \brief Fetches internal data in a separate thread
template <typename T, typename E = Error>
class AsyncTaskRunner {
public:
    using Task          = std::function<Result<T, E>()>;
    using TaskCallback  = std::function<void(T&&)>;
    using ErrorCallback = std::function<void(E&&)>;

    /// \brief Creates and AsyncTaskRunner
    /// \param task_ready_callback - Gets called from another thread when tasks are ready.
    explicit AsyncTaskRunner(NotifyCallback task_ready_callback = nullptr);

    /// \brief Kills the task loop and waits for the task thread to exit.
    ~AsyncTaskRunner();

    /// \brief Schedule an asynchronous task.
    /// \param task - the task that will be run in a separate thread
    /// \param on_completion - called on the same thread as `invoke_callbacks_for_fetched_meshes`
    /// \param on_error - called on the same thread as `invoke_callbacks_for_fetched_meshes`
    auto schedule_task(Task task, TaskCallback on_completion = nullptr, ErrorCallback on_error = nullptr) -> void;

    /// \brief Processes completed tasks by invoking their `on_completion` or `on_error` callbacks.
    /// \param max_updates - the maximum number of callbacks to invoke
    /// \return true if there are still completed tasks that haven't been processed
    auto invoke_callbacks_for_finished_tasks(std::size_t max_updates = 10ul) -> bool;

    /// \brief Waits for a task to complete then invokes it's callbacks. This function will
    ///        not block if there are already completed tasks waiting to be processed.
    auto invoke_next_callback_blocking() -> void;

    /// \brief Returns true if tasks are running on the work thread.
    [[nodiscard]] auto processing() const -> bool;

private:
    struct TaskToDo {
        Task          task;
        TaskCallback  on_completion;
        ErrorCallback on_error;

        explicit TaskToDo(Task task_to_do, TaskCallback on_completion_callback, ErrorCallback on_error_callback)
            : task(std::move(task_to_do)),
              on_completion(std::move(on_completion_callback)),
              on_error(std::move(on_error_callback)) {}
    };

    struct FinishedTask {
        Result<T, E>  result;
        TaskCallback  on_completion;
        ErrorCallback on_error;

        explicit FinishedTask(Result<T, E>  task_result,
                              TaskCallback  on_completion_callback,
                              ErrorCallback on_error_callback)
            : result(std::move(task_result)),
              on_completion(std::move(on_completion_callback)),
              on_error(std::move(on_error_callback)) {}
    };

    BlockingQueue<std::unique_ptr<TaskToDo>> tasks_to_do_;
    BlockingQueue<FinishedTask>              finished_tasks_;

    std::thread      task_thread_;
    std::atomic_bool processing_ = false;

    auto task_run_loop() -> void;
};

template <typename T, typename E>
AsyncTaskRunner<T, E>::AsyncTaskRunner(NotifyCallback task_ready_callback)
    : finished_tasks_(task_ready_callback), task_thread_([this] { task_run_loop(); }) {}

template <typename T, typename E>
AsyncTaskRunner<T, E>::~AsyncTaskRunner() {
    tasks_to_do_.clear();
    tasks_to_do_.emplace_back(nullptr); // This forces the task run loop to exit.
    task_thread_.join();
}

template <typename T, typename E>
auto AsyncTaskRunner<T, E>::schedule_task(Task task, TaskCallback on_completion, ErrorCallback on_error) -> void {
    if (task == nullptr) {
        throw std::invalid_argument("Task functors cannot be null");
    }
    auto task_to_do = std::make_unique<TaskToDo>(std::move(task), std::move(on_completion), std::move(on_error));
    tasks_to_do_.emplace_back(std::move(task_to_do));
}

template <typename T, typename E>
auto AsyncTaskRunner<T, E>::invoke_callbacks_for_finished_tasks(std::size_t max_updates) -> bool {
    for (auto i = 0ul; i < max_updates && !finished_tasks_.empty(); ++i) {
        invoke_next_callback_blocking();
    }
    return !finished_tasks_.empty();
}

template <typename T, typename E>
auto AsyncTaskRunner<T, E>::invoke_next_callback_blocking() -> void {
    FinishedTask finished_task = finished_tasks_.pop_front();
    finished_task
        .result
        // call `on_completion` if successful
        .map([&finished_task](T value) {
            if (finished_task.on_completion) {
                finished_task.on_completion(std::move(value));
            }
        })
        // call `on_error` if there was an error
        .map_error([&finished_task](E error) {
            if (finished_task.on_error) {
                finished_task.on_error(std::move(error));
            }
        });
}

template <typename T, typename E>
auto AsyncTaskRunner<T, E>::processing() const -> bool {
    return processing_;
}

template <typename T, typename E>
auto AsyncTaskRunner<T, E>::task_run_loop() -> void {
    while (std::unique_ptr<TaskToDo> task_to_do = tasks_to_do_.pop_front()) {
        processing_ = true;
        finished_tasks_.emplace_back(task_to_do->task(),
                                     std::move(task_to_do->on_completion),
                                     std::move(task_to_do->on_error));
        processing_ = false;
    }
}

} // namespace ltb::util
