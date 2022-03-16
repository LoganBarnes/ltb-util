// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/string.hpp"

// external
#include <doctest/doctest.h>

// standard
#include <algorithm>
#include <cctype>

namespace ltb::util {

auto starts_with(std::string const& str, std::string const& prefix) -> bool {
    return str.compare(0, prefix.length(), prefix) == 0;
}

TEST_CASE("[ltb][util][string] string_starts_with") {
    CHECK(starts_with("", ""));
    CHECK(starts_with("check for prefix", ""));
    CHECK(starts_with("check for prefix", "c"));
    CHECK(starts_with("check for prefix", "check"));
    CHECK(starts_with("check for prefix", "check "));
    CHECK(starts_with("check for prefix", "check for prefix"));

    CHECK_FALSE(starts_with("", "?"));
    CHECK_FALSE(starts_with("check for prefix", "z"));
    CHECK_FALSE(starts_with("check for prefix", "checkk"));
    CHECK_FALSE(starts_with("check for prefix", "check  "));
    CHECK_FALSE(starts_with("check for prefix", "check for prefix?"));
}

auto to_lower_ascii(std::string const& str) -> std::string {
    auto result = str;
    std::transform(str.begin(), str.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace ltb::util
