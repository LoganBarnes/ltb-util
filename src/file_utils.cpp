// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/file_utils.hpp"

// standard
#include <fstream>

namespace ltb::util {

auto read_file_to_string(std::filesystem::path const& filename) -> util::Result<std::string> {
    auto input_stream = std::ifstream(filename);

    if (!input_stream.is_open()) {
        return tl::make_unexpected(LTB_MAKE_ERROR("Failed to open: '" + filename.string() + "'"));
    }

    input_stream.seekg(0, std::ios::end);
    auto size = input_stream.tellg();
    if (size == -1) {
        return {};
    }
    auto buffer = std::string(static_cast<std::size_t>(size), ' ');
    input_stream.seekg(0);
    input_stream.read(&buffer[0], size);

    return buffer;
}

} // namespace ltb::util
