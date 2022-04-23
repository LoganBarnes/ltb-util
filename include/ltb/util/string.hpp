// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

namespace ltb::util {

auto starts_with(std::string const& str, std::string const& prefix) -> bool;

auto to_lower_ascii(std::string const& str) -> std::string;

} // namespace ltb::util
