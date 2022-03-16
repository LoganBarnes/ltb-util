// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/result.hpp"

// project
#include "ltb/util/error_callback.hpp"
#include "ltb/util/ignore.hpp"

// external
#include <doctest/doctest.h>

// standard
#include <iostream>

namespace ltb::util {

auto success() -> Result<void> {
    return {};
}

} // namespace ltb::util

namespace {

using namespace ltb;

// function that returns an int or 'ltb::util::Error'
[[maybe_unused]] auto my_function(int non_negative) -> util::Result<int> {
    if (non_negative < 0) {
        return tl::make_unexpected(LTB_MAKE_ERROR("number is negative"));
    }
    return non_negative; // Success
}

TEST_CASE("[ltb][util][result] Result compilation check") {
    // my_function(0); // compilation error since the return value is not used (C++17)

    // auto result1 = my_function(1); // compilation error since 'result1' is unused

    auto result2 = my_function(2); // OK, result is used
    if (result2) {}

    CHECK_THROWS_AS(my_function(-1).or_else(util::throw_error<>).value(), std::runtime_error);
    CHECK_THROWS_AS(my_function(-42).or_else(util::throw_error<std::invalid_argument>).value(), std::invalid_argument);

    my_function(3).value(); // OK, throws on error
    my_function(4).or_else(util::throw_error<>).value(); // OK, clearer error handling
    my_function(5).map([](auto i) { return std::to_string(i); }).value_or(std::string("NEG")); // OK

    util::ignore(my_function(6)); // OK, but not recommended

    auto result3 = my_function(7); // OK, but not recommended
    ltb::util::ignore(result3);
}

struct A {};
struct B {};
struct C {};

[[maybe_unused]] auto a(A) -> util::Result<B> {
    return B{};
}
[[maybe_unused]] auto b(B const&) -> util::Result<C> {
    return C{};
}
[[maybe_unused]] auto c(C&&) -> util::Result<void> {
    return util::success();
}

TEST_CASE("[ltb][util][result] Result::and_then") {
    CHECK(a(A{}).and_then(b).and_then(c).has_value());
}

} // namespace
