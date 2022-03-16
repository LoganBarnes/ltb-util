// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/uuid.hpp"

// project
#include "ltb/util/hash_utils.hpp"

// external
#include <doctest/doctest.h>

namespace ltb::util {
namespace {

using Type1Uuid = Uuid<struct Type1>;
using Type2Uuid = Uuid<struct Type2>;
using Type3Uuid = Uuid<struct Type3>;

} // namespace

UuidGenerator::UuidGenerator() : random_generator_(std::random_device{}()), uuid_generator_(random_generator_) {}

UuidGenerator::UuidGenerator(std::string const& seed)
    : random_generator_(string_seed_to_uint(seed)), uuid_generator_(random_generator_) {}

TEST_CASE("[ltb][util][uuid] Test the UuidGenerator functionality") {
    auto generator = UuidGenerator{};

    CHECK_FALSE(generator.generate<Type1Uuid>() == generator.generate<Type1Uuid>());
    CHECK_FALSE(generator.generate<Type2Uuid>() == generator.generate<Type2Uuid>());
    CHECK_FALSE(generator.generate<Type3Uuid>() == generator.generate<Type3Uuid>());
}

} // namespace ltb::util
