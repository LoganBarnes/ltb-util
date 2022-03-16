// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ltb::util {

template <typename Map, typename Key>
auto has_key(Map const& map, Key const& key) -> bool {
    return map.find(key) != map.end();
}

template <typename V, typename T>
auto has_item(V& v, const T& item) {
    return std::find(std::begin(v), std::end(v), item) != std::end(v);
}

template <typename V, typename T>
auto remove_all_by_value(V& v, const T& value) {
    return v.erase(std::remove(std::begin(v), std::end(v), value), std::end(v));
}

template <typename V, typename Pred>
auto remove_all_by_predicate(V& v, Pred&& pred) {
    return v.erase(std::remove_if(std::begin(v), std::end(v), std::forward<Pred>(pred)), std::end(v));
}

} // namespace ltb::util
