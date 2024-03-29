// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace ltb::util {
namespace detail {

auto demangle(char const* name) -> std::string;
auto replace_ugly_strings(std::string type_str) -> std::string;

} // namespace detail

template <typename T>
auto type_string(const T&) -> std::string {
    return detail::replace_ugly_strings(detail::demangle(typeid(T).name()));
}

template <typename T>
auto type_string() -> std::string {
    return detail::replace_ugly_strings(detail::demangle(typeid(T).name()));
}

} // namespace ltb::util
