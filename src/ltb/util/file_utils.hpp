// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/util/result.hpp"

// standard
#include <filesystem>

namespace ltb::util {

auto read_file_to_string(std::filesystem::path const& filename) -> util::Result<std::string>;

} // namespace ltb::util
