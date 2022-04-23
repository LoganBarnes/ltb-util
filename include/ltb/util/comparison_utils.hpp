// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef __CUDACC
#define GOOGLE_COMPARISON
#endif

#ifdef GOOGLE_COMPARISON
#include "google/AlmostEquals.h"
#include <cstddef> // std::size_t
#else
#include <cmath>
#include <limits>
#endif

// standard
#include <type_traits>

namespace ltb::util {

#ifdef GOOGLE_COMPARISON

template <typename T>
auto constexpr almost_equal(T a, T b) -> typename std::enable_if_t<std::is_floating_point<T>::value, bool> {
    return AlmostEquals(a, b);
}

#else

template <typename T>
auto constexpr almost_equal(T a, T b, T scale = 1.0, T epsilon = 10 * std::numeric_limits<T>::epsilon()) ->
    typename std::enable_if_t<std::is_floating_point_v<T>, bool> {
    // exact comparison allows to handle infinities and prevents from getting FPE triggered by (inf - inf)
    bool const exactly_equal = (a == b);
    return exactly_equal || std::abs(a - b) < epsilon * (scale + std::abs(a) + std::abs(b));
}

#endif

} // namespace ltb::util
