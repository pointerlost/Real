//
// Created by pointerlost on 11/6/25.
//

/*
 *  This page is used to store "Real Engine" types
 */

#pragma once
#include <map>
#include <unordered_map>
#include <utility>

namespace Real {

    template <typename First, typename Second>
    using pair = std::pair<First, Second>;

    template <typename Key, typename Val>
    using map = std::map<Key, Val>;

    template <typename Key, typename Val>
    using unordered_map = std::unordered_map<Key, Val>;
}
