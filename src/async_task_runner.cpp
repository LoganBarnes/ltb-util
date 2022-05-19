// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/async_task_runner.hpp"

// external
#include <doctest/doctest.h>

// standard
#include <array>
#include <chrono>

namespace {

using namespace std::chrono_literals;

TEST_CASE("[ltb][util][async_task_runner] AsyncTaskRunner functions with no callbacks") {
    auto task_runner = ltb::util::AsyncTaskRunner<bool>{};

    task_runner.schedule_task([] { return true; });
    task_runner.schedule_task([] { return false; });

    CHECK_NOTHROW(task_runner.invoke_next_callback_blocking());
    CHECK_NOTHROW(task_runner.invoke_next_callback_blocking());
}

TEST_CASE("[ltb][util][async_task_runner] AsyncTaskRunner calls correct callbacks") {
    auto task_runner = ltb::util::AsyncTaskRunner<int>{};

    int successes = 0;
    int fails     = 0;

    {
        auto on_success = [&successes](auto&&) { ++successes; };
        auto on_error   = [&fails](auto&&) { ++fails; };
        auto just_throw = [](auto&&) { throw std::bad_function_call(); }; // because why not

        task_runner.schedule_task([] { return 1; }, on_success, on_error);
        task_runner.schedule_task([] { return tl::make_unexpected(LTB_MAKE_ERROR("OMG!!!")); }, on_success, on_error);
        task_runner.schedule_task([] { return 3; }, just_throw, just_throw);
    }

    task_runner.invoke_next_callback_blocking(); // Should invoke only the success callback
    CHECK(successes == 1);
    CHECK(fails == 0);

    task_runner.invoke_next_callback_blocking(); // Should invoke only the error callback
    CHECK(successes == 1);
    CHECK(fails == 1);

    CHECK_THROWS_AS(task_runner.invoke_next_callback_blocking(), std::bad_function_call);
}

TEST_CASE("[ltb][util][async_task_runner] AsyncTaskRunner runs tasks in order") {
    auto task_runner = ltb::util::AsyncTaskRunner<std::chrono::system_clock::time_point>{};

    // Sleep for a bit then return the current time (this will happen on another thread).
    auto task = [] {
        std::this_thread::sleep_for(10ms);
        return std::chrono::system_clock::now();
    };
    // There should be no errors because we don't return any
    auto on_error = [](auto&&) { REQUIRE(false); };

    auto times = std::array<std::chrono::system_clock::time_point, 5>{};

    task_runner.schedule_task(
        task, [&times](auto result) { times[0] = result; }, on_error);
    task_runner.schedule_task(
        task, [&times](auto result) { times[1] = result; }, on_error);
    task_runner.schedule_task(
        task, [&times](auto result) { times[2] = result; }, on_error);
    task_runner.schedule_task(
        task, [&times](auto result) { times[3] = result; }, on_error);
    task_runner.schedule_task(
        task, [&times](auto result) { times[4] = result; }, on_error);

    task_runner.invoke_next_callback_blocking();
    task_runner.invoke_next_callback_blocking();
    task_runner.invoke_next_callback_blocking();
    task_runner.invoke_next_callback_blocking();
    task_runner.invoke_next_callback_blocking();

    for (auto i = 1ul; i < times.size(); ++i) {
        CHECK(times[i - 1ul] < times[i]);
    }
}

TEST_CASE("[ltb][util][async_task_runner][slow] AsyncTaskRunner runs tasks asynchronously") {
    auto task_runner = ltb::util::AsyncTaskRunner<std::chrono::system_clock::time_point>{};

    auto task_time         = std::chrono::system_clock::time_point{};
    auto invoke_time       = std::chrono::system_clock::time_point{};
    auto before_sleep_time = std::chrono::system_clock::time_point{};
    auto after_sleep_time  = std::chrono::system_clock::time_point{};
    auto after_invoke_time = std::chrono::system_clock::time_point{};

    task_runner.schedule_task(
        // This will run on another thread
        [] {
            std::this_thread::sleep_for(500ms);
            return std::chrono::system_clock::now();
        },
        // This will run when we call `invoke_next_callback_blocking`
        [&task_time, &invoke_time](auto&& result) {
            task_time   = result;
            invoke_time = std::chrono::system_clock::now();
        },
        [](auto&&) {
            REQUIRE(false); // There should be no errors because we don't return any
        });

    before_sleep_time = std::chrono::system_clock::now();
    std::this_thread::sleep_for(1000ms);
    after_sleep_time = std::chrono::system_clock::now();

    task_runner.invoke_next_callback_blocking();

    after_invoke_time = std::chrono::system_clock::now();

    CHECK(before_sleep_time <= task_time);
    CHECK(task_time <= after_sleep_time);
    CHECK(after_sleep_time <= invoke_time);
    CHECK(invoke_time <= after_invoke_time);
}

TEST_CASE("[ltb][util][async_task_runner] AsyncTaskRunner runs tasks one at a time") {
    auto task_runner = ltb::util::AsyncTaskRunner<std::chrono::system_clock::time_point>{};

    auto task = [] {
        std::this_thread::sleep_for(100ms);
        return std::chrono::system_clock::now();
    };

    [[maybe_unused]] auto start = std::chrono::system_clock::now();

    task_runner.schedule_task(task);
    task_runner.schedule_task(task);
    task_runner.schedule_task(task);
    task_runner.schedule_task(task);
    task_runner.schedule_task(task);

    [[maybe_unused]] auto end = std::chrono::system_clock::now();
    CHECK(end - start < 500ms); // Tasks are run asynchronously

    task_runner.invoke_next_callback_blocking();
    task_runner.invoke_next_callback_blocking();
    task_runner.invoke_next_callback_blocking();
    task_runner.invoke_next_callback_blocking();
    task_runner.invoke_next_callback_blocking();

    end = std::chrono::system_clock::now();
    CHECK(end - start > 500ms); // Tasks are run one at a time
}

TEST_CASE("[ltb][util][async_task_runner] AsyncTaskRunner kills task thread on destruction") {

    auto start = std::chrono::system_clock::time_point{};
    auto end   = std::chrono::system_clock::time_point{};
    {
        auto task_runner = ltb::util::AsyncTaskRunner<std::chrono::system_clock::time_point>{};

        auto task = [] {
            std::this_thread::sleep_for(100ms);
            return std::chrono::system_clock::now();
        };

        start = std::chrono::system_clock::now();

        task_runner.schedule_task(task);
        task_runner.schedule_task(task);
        task_runner.schedule_task(task);
        task_runner.schedule_task(task);
        task_runner.schedule_task(task);

        // Should kill async thread and prevent stop remaining tasks on destruction
    }
    // If not all the tasks were run before the thread was destroyed then
    // we should reach this point sooner than 500ms. If all five tasks had
    // time to run then we won't reach this point for at least 500ms.

    end = std::chrono::system_clock::now();
    CHECK(end - start < 500ms);
}

TEST_CASE("[ltb][util][async_task_runner] AsyncTaskRunner only processes up to max_updates") {
    struct Ignored {};
    static auto block_until_a_task_completes = ltb::util::BlockingQueue<Ignored>{};

    auto task_complete_callback = [] { block_until_a_task_completes.emplace_back(Ignored{}); };

    auto task_runner = ltb::util::AsyncTaskRunner<bool>{task_complete_callback};

    // Return values don't matter; we don't use them.
    task_runner.schedule_task([] { return true; });
    task_runner.schedule_task([] { return true; });
    task_runner.schedule_task([] { return true; });
    task_runner.schedule_task([] { return true; });
    task_runner.schedule_task([] { return true; });
    task_runner.schedule_task([] { return true; });

    auto tasks_to_complete = 6; // Block until all tasks have been processed.
    while (tasks_to_complete > 0) {
        block_until_a_task_completes.pop_front();
        --tasks_to_complete;
    }

    SUBCASE("default") {
        // Process all the tasks and return false because there are no more to process
        CHECK_FALSE(task_runner.invoke_callbacks_for_finished_tasks());
    }

    SUBCASE("all at once") {
        // Process the all the tasks and return false because there are no more to process
        CHECK_FALSE(task_runner.invoke_callbacks_for_finished_tasks(6));
    }

    SUBCASE("in chunks") {
        // Process the first 2 tasks and return true because there are more to process
        CHECK(task_runner.invoke_callbacks_for_finished_tasks(3));

        // Process the next 2 tasks and return true because there are more to process
        CHECK(task_runner.invoke_callbacks_for_finished_tasks(2));

        // Process the last task and return false because there are no more to process
        CHECK_FALSE(task_runner.invoke_callbacks_for_finished_tasks(1));
    }

    SUBCASE("one at a time") {
        CHECK(task_runner.invoke_callbacks_for_finished_tasks(1));
        CHECK(task_runner.invoke_callbacks_for_finished_tasks(1));
        CHECK(task_runner.invoke_callbacks_for_finished_tasks(1));
        CHECK(task_runner.invoke_callbacks_for_finished_tasks(1));
        CHECK(task_runner.invoke_callbacks_for_finished_tasks(1));

        // Process the last task and return false because there are no more to process
        CHECK_FALSE(task_runner.invoke_callbacks_for_finished_tasks(1));
    }
}

} // namespace
