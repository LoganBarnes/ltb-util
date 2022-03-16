// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// standard
#include <functional>
#include <string>

namespace ltb::util {

///
/// https://stackoverflow.com/a/2595226 ...ish.
///
template <class T>
auto hash_combine(std::size_t const& seed, const T& v) -> std::size_t {
    std::hash<T> hasher;
    return seed ^ (hasher(v) + 0x9e3779b9u + (seed << 6u) + (seed >> 2u));
}

/// \brief Convert a string to a uint32_t using `std::seed_seq`.
auto string_seed_to_uint(std::string const& str) -> std::uint32_t;

} // namespace ltb::util
