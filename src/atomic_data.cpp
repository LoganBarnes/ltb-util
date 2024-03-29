// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/atomic_data.hpp"

// external
#include <doctest/doctest.h>

// standard
#include <array>
#include <chrono>
#include <tuple>
#include <vector>

namespace {

using namespace std::chrono_literals;

TEST_CASE("[ltb][util][atomic] atomic_data_wait_timeout") {
    using namespace ltb;

    struct SharedData {
        bool stop_waiting    = false;
        bool update_happened = false;
    };
    util::AtomicData<SharedData> shared_data;

    // Sleep for 2 seconds then notify the main thread
    std::thread thread([&] {
        std::this_thread::sleep_for(1s);
        shared_data.use_safely([](auto& data) { data.stop_waiting = true; });
        shared_data.notify_one();
    });

    // Wait for at most 100ms and exit with false if the timeout is reached
    CHECK_FALSE(shared_data.wait_to_use_safely(
        100ms, [](auto const& data) { return data.stop_waiting; }, [](auto& data) { data.update_happened = true; }));

    // Since the timeout of 100ms was reached the update should not have happened
    shared_data.use_safely([](auto const& data) {
        [[maybe_unused]] bool should_be_false = data.update_happened;
        CHECK_FALSE(should_be_false);
    });

    // Wait again up to 5 seconds (the other thread should wake up and notify us within that time)
    CHECK(shared_data.wait_to_use_safely(
        5s, [](auto const& data) { return data.stop_waiting; }, [](auto& data) { data.update_happened = true; }));

    thread.join();

    // The update should happen because we were notified before reaching the second timeout
    shared_data.use_safely([&]([[maybe_unused]] SharedData const& data) { CHECK(data.update_happened); });
}

TEST_CASE("[ltb][util][atomic] atomic_data_notify_all") {
    using namespace ltb;

    struct SharedData {
        bool write_away  = false;
        int  num_threads = 0;
    };
    util::AtomicData<SharedData> shared_data;

    std::array<std::thread, 50> threads;

    // Wait for 'notify_all' from the main thread then safely increment 'num_threads' once per thread
    for (auto& thread : threads) {
        thread = std::thread([&] {
            shared_data.wait_to_use_safely([](const auto& data) { return data.write_away; },
                                           [](auto& data) { ++data.num_threads; });
        });
    }

    // notify all the threads at once
    shared_data.use_safely([](auto& data) { data.write_away = true; });
    shared_data.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }

    // 'num_threads' should exactly equal the amount of threads we created
    shared_data.use_safely([&]([[maybe_unused]] SharedData const& data) { CHECK(data.num_threads == threads.size()); });
}

TEST_CASE_TEMPLATE("[ltb][util][atomic] interleaved_atomic_data", T, short, int, unsigned, float, double) {
    using namespace ltb;

    struct SharedData {
        T              current_number = 0;
        std::vector<T> all_data       = {};
        std::vector<T> odds           = {};
        std::vector<T> even           = {};
        bool           writing_odds   = false;
    };

    auto const max_number = T(9);

    // Use AtomicData structure to update data from two threads
    util::AtomicData<SharedData> shared_data;

    // Write odd numbers
    auto thread = std::thread([&] {
        bool stop_loop = false;
        do {
            shared_data.wait_to_use_safely([](const SharedData& data) { return data.writing_odds; },
                                           [&](SharedData& data) {
                                               data.all_data.emplace_back(data.current_number++);
                                               data.odds.emplace_back(data.all_data.back());
                                               stop_loop         = data.current_number >= max_number;
                                               data.writing_odds = false;
                                           });
            shared_data.notify_one();
        } while (!stop_loop);
    });

    // Write even numbers
    bool stop_loop;
    do {
        shared_data.wait_to_use_safely([](SharedData const& data) { return !data.writing_odds; },
                                       [&](SharedData& data) {
                                           data.all_data.emplace_back(data.current_number++);
                                           data.even.emplace_back(data.all_data.back());
                                           stop_loop         = data.current_number >= max_number;
                                           data.writing_odds = true;
                                       });
        shared_data.notify_one();
    } while (!stop_loop);

    thread.join();

    // shared_data should be correctly ordered despite being modified by separate threads
    shared_data.use_safely([&]([[maybe_unused]] SharedData const& data) {
        CHECK(data.current_number == max_number + 1);
        CHECK(data.even == std::vector<T>{0, 2, 4, 6, 8});
        CHECK(data.odds == std::vector<T>{1, 3, 5, 7, 9});
        CHECK(data.all_data == std::vector<T>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
        CHECK_FALSE(data.writing_odds);
    });
}

} // namespace
