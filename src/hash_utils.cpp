// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/hash_utils.hpp"

// standard
#include <random>

namespace ltb::util {

auto string_seed_to_uint(std::string const& str) -> std::uint32_t {
    auto seed_seq = std::seed_seq(str.begin(), str.end());
    auto seed     = std::vector<std::uint32_t>(1ul);
    seed_seq.generate(seed.begin(), seed.end());
    return seed.back();
}

} // namespace ltb::util
