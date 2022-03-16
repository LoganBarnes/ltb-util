// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/util/error.hpp"

#if defined(LTB_EXPECTED) && defined(__GNUC__) && (__GNUC__ >= 4)
#define CHECK_RESULT __attribute__((warn_unused))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define CHECK_RESULT _Check_return_
#else
#define CHECK_RESULT
#endif

namespace tl {

template <class T, class E>
class CHECK_RESULT expected;

}

// external
#include <tl/expected.hpp>

// standard
#include <stdexcept>

namespace ltb::util {

/**
 * @brief A type of tl::expected that requires the result to be used.
 *
 * Example usage:
 *
 * @code{.cpp}

    // function that returns an int or 'ltb::util::Error'
    auto my_function(int non_negative) -> util::Result<int> {
        if (non_negative < 0) {
            return tl::make_unexpected(LTB_MAKE_ERROR("number is negative"));
        }
        return non_negative; // Success
    }

    int main() {
        my_function(0); // compilation error since the return value is not used (C++17)

        auto result1 = my_function(1); // compilation error since 'result1' is unused

        auto result2 = my_function(2); // OK, result is used
        if (result2) {
        }

        my_function(3).value(); // OK, throws on error
        my_function(4).or_else(util::throw_error<>).value(); // OK, clearer error handling
        my_function(5).map([](auto i) { return std::to_string(i); }).value_or(std::string("NEG")); // OK

        util::ignore(my_function(6)); // OK, but not recommended

        auto result3 = my_function(7); // OK, but not recommended
        ltb::util::ignore(result3);

        return 0;
    }

 * @endcode
 *
 */
template <typename T, typename E = ::ltb::util::Error>
using Result = tl::expected<T, E>;

auto success() -> Result<void>;

} // namespace ltb::util
