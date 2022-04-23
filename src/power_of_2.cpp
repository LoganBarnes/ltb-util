// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/power_of_2.hpp"

// external
#include <doctest/doctest.h>

// standard
#include <limits>

namespace ltb::util {
namespace {

// Custom uint64 literal
auto operator"" _u64(unsigned long long value) {
    return std::uint64_t{value};
}

} // namespace

auto next_power_of_2(std::uint64_t value) -> std::uint64_t {
    value--;
    value |= value >> 1_u64;
    value |= value >> 2_u64;
    value |= value >> 4_u64;
    value |= value >> 8_u64;
    value |= value >> 16_u64;
    value |= value >> 32_u64;
    value++;
    return value;
}

TEST_CASE("[ltb][util][power_2] next_power_of_2") {
    CHECK(next_power_of_2(0_u64) == 0_u64); // Some underflow/overflow happening here :/

    // Every number up to 2^4:

    // 2^0
    CHECK(next_power_of_2(1_u64) == 1_u64);
    // 2^1
    CHECK(next_power_of_2(2_u64) == 2_u64);
    // 2^2
    CHECK(next_power_of_2(3_u64) == 4_u64);
    CHECK(next_power_of_2(4_u64) == 4_u64);
    // 2^3
    CHECK(next_power_of_2(5_u64) == 8_u64);
    CHECK(next_power_of_2(6_u64) == 8_u64);
    CHECK(next_power_of_2(7_u64) == 8_u64);
    CHECK(next_power_of_2(8_u64) == 8_u64);
    // 2^4
    CHECK(next_power_of_2(9_u64) == 16_u64);
    CHECK(next_power_of_2(10_u64) == 16_u64);
    CHECK(next_power_of_2(11_u64) == 16_u64);
    CHECK(next_power_of_2(12_u64) == 16_u64);
    CHECK(next_power_of_2(13_u64) == 16_u64);
    CHECK(next_power_of_2(14_u64) == 16_u64);
    CHECK(next_power_of_2(15_u64) == 16_u64);
    CHECK(next_power_of_2(16_u64) == 16_u64);

    // All powers of 2 after 2^4 with the numbers before and after.

    REQUIRE((1_u64 << 5_u64) == 32_u64); // Sanity check

    for (auto power = 5_u64; power < 64_u64; ++power) {
        [[maybe_unused]] auto prev = 1_u64 << (power - 1_u64);
        [[maybe_unused]] auto curr = 1_u64 << (power - 0_u64);

        CHECK(next_power_of_2(prev + 1_u64) == curr);
        CHECK(next_power_of_2(curr - 1_u64) == curr);
        CHECK(next_power_of_2(curr + 0_u64) == curr);
    }

    // 2^64 (overflow)
    CHECK(next_power_of_2((1_u64 << 63_u64) + 1_u64) == 0_u64);
    CHECK(next_power_of_2(std::numeric_limits<std::uint64_t>::max()) == 0_u64);
}

} // namespace ltb::util
