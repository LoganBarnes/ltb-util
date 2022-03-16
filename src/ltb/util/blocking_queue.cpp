// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////

// project
#include "ltb/util/blocking_queue.hpp"

// external
#include <doctest/doctest.h>

// standard
#include <thread>

namespace {
using namespace ltb;

TEST_CASE("[ltb][util][blocking_queue] empty_is_empty") {
    util::BlockingQueue<char> bq;
    CHECK(bq.size() == 0);
    CHECK(bq.empty());

    bq.emplace_back('$');

    CHECK(bq.size() == 1);
    CHECK_FALSE(bq.empty());
}

// The test is inspired by memory management of dataflow_ptr and stacks of those.
TEST_CASE("[ltb][util][blocking_queue] not_reentering_mutex_on_clear") {
    struct NastyObject {
        NastyObject() = default;
        explicit NastyObject(util::BlockingQueue<NastyObject>& queue) : queue_{&queue} {}
        ~NastyObject() {
            if (queue_) {
                queue_->push_back(NastyObject{});
            }
        }

        util::BlockingQueue<NastyObject>* queue_ = nullptr;
    };

    util::BlockingQueue<NastyObject> queue;
    queue.push_back(NastyObject{queue});
    queue.clear();

    // Actually if we reached here and didn't deadlock, it's amazing!
    REQUIRE(queue.size() == 1);

    queue.clear();
    CHECK(queue.empty());
}

TEST_CASE_TEMPLATE("[ltb][util][blocking_queue] interleaved_data", TestType, short, int, float, double) {
    std::vector<TestType> shared_data;

    // Use two blocking queues as control structures between two threads
    util::BlockingQueue<TestType> even_bq;
    util::BlockingQueue<TestType> odds_bq;

    // Wait for the next even number from 'even_bq',
    // Write the number to 'shared_data',
    // Write the next odd number to 'odds_bq'
    std::thread thread([&] {
        for (short i = 1; i < 10; i += 2) {
            TestType to_add = even_bq.pop_front();
            shared_data.emplace_back(to_add);
            odds_bq.push_back(i);
        }
    });

    // Write the next even number to 'even_bq'
    // Wait for the next odd number from 'odds_bq',
    // Write the number to 'shared_data',
    for (short i = 0; i < 10; i += 2) {
        even_bq.push_back(i);
        TestType to_add = odds_bq.pop_front();
        shared_data.emplace_back(to_add);
    }

    thread.join();

    // All the data should be processed by now
    CHECK(even_bq.empty());
    CHECK(odds_bq.empty());

    // shared_data should be correctly ordered despite being modified by separate threads
    CHECK(shared_data == std::vector<TestType>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
}

} // namespace
