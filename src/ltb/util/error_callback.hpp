// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/util/error.hpp"

// standard
#include <functional>
#include <stdexcept>

namespace ltb::util {

using ErrorCallback = std::function<void(Error)>;

constexpr auto ignore_errors = nullptr;

template <typename E = std::runtime_error>
auto throw_error(Error const& error) -> void {
    throw E(error.debug_error_message());
}

auto invoke_if_non_null(ErrorCallback const& callback, Error error) -> void;

} // namespace ltb::util
