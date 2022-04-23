// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/error_callback.hpp"

// external
#include <doctest/doctest.h>

namespace ltb::util {

auto invoke_if_non_null(ErrorCallback const& callback, Error error) -> void {
    if (callback) {
        callback(std::move(error));
    }
}

TEST_CASE("[ltb][util][error] invoke_if_non_null") {
    CHECK_NOTHROW(invoke_if_non_null(ignore_errors, LTB_MAKE_ERROR("Ignored Error")));
    CHECK_THROWS_AS(invoke_if_non_null(throw_error<>, LTB_MAKE_ERROR("Runtime Error")), std::runtime_error);
    CHECK_THROWS_AS(invoke_if_non_null(throw_error<std::invalid_argument>, LTB_MAKE_ERROR("Argument Error")),
                    std::invalid_argument);

    auto error_message  = std::string{};
    auto error_callback = [&error_message](Error const& error) { error_message = error.error_message(); };
    CHECK_NOTHROW(invoke_if_non_null(error_callback, LTB_MAKE_ERROR("Callback Error")));
    CHECK(error_message == "Callback Error");
}

} // namespace ltb::util
