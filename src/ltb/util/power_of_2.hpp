// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// standard
#include <cstdint>

namespace ltb::util {

/// \brief The next power of 2 greater than or equal to `value`.
auto next_power_of_2(std::uint64_t value) -> std::uint64_t;

} // namespace ltb::util
