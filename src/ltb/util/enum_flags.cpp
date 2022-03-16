// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/enum_flags.hpp"

// external
#include <doctest/doctest.h>

namespace {

using namespace ltb;

TEST_CASE("[ltb][util][enum_bits] default conversions and static asserts") {
    enum class Enum : std::uint8_t {
        Flag1,
        Flag2,
        Flag3,
        Flag4,
        Flag5,
        Flag6,
        Flag7,
        Flag8,
        // Flag9, // Uncomment this value to trigger a static assert
    };

    auto              flag0 = util::Flags<Enum>{};
    util::Flags<Enum> flag1 = Enum::Flag1;
    auto              flag2 = util::Flags{Enum::Flag2};
    auto              flag3 = util::Flags{Enum::Flag3};
    auto              flag4 = util::Flags{Enum::Flag4};
    auto              flag5 = util::Flags{Enum::Flag5};
    auto              flag6 = util::Flags{Enum::Flag6};
    auto              flag7 = util::Flags{Enum::Flag7};
    auto              flag8 = util::Flags{Enum::Flag8};

    auto flags = util::Flags<Enum>{0b01100101};

    CHECK(flag0.bits == 0b00000000);
    CHECK(flag1.bits == 0b00000001);
    CHECK(flag2.bits == 0b00000010);
    CHECK(flag3.bits == 0b00000100);
    CHECK(flag4.bits == 0b00001000);
    CHECK(flag5.bits == 0b00010000);
    CHECK(flag6.bits == 0b00100000);
    CHECK(flag7.bits == 0b01000000);
    CHECK(flag8.bits == 0b10000000);

    CHECK(flags.bits == 0b01100101);
}

TEST_CASE("[ltb][util][enum_bits] enums as flags via bit manipulation") {
    enum class Enum : std::uint16_t {
        Flag1,
        Flag2,
        Flag3,
    };

    auto flags = util::Flags<Enum>{};
    CHECK(flags.bits == 0);

    CHECK_FALSE(util::has_flag(flags, Enum::Flag1));
    CHECK_FALSE(util::has_flag(flags, Enum::Flag2));
    CHECK_FALSE(util::has_flag(flags, Enum::Flag3));

    flags = Enum::Flag1;
    CHECK(flags.bits == 0b0001);
    CHECK(util::has_flag(flags, Enum::Flag1));
    CHECK_FALSE(util::has_flag(flags, Enum::Flag2));
    CHECK_FALSE(util::has_flag(flags, Enum::Flag3));

    flags = Enum::Flag2;
    CHECK(flags.bits == 0b0010);
    CHECK_FALSE(util::has_flag(flags, Enum::Flag1));
    CHECK(util::has_flag(flags, Enum::Flag2));
    CHECK_FALSE(util::has_flag(flags, Enum::Flag3));

    flags = Enum::Flag3;
    CHECK(flags.bits == 0b0100);
    CHECK_FALSE(util::has_flag(flags, Enum::Flag1));
    CHECK_FALSE(util::has_flag(flags, Enum::Flag2));
    CHECK(util::has_flag(flags, Enum::Flag3));

    flags = util::make_flags(Enum::Flag1, Enum::Flag2);
    CHECK(flags.bits == 0b0011);
    CHECK(util::has_flag(flags, Enum::Flag1));
    CHECK(util::has_flag(flags, Enum::Flag2));
    CHECK_FALSE(util::has_flag(flags, Enum::Flag3));

    flags = util::make_flags(Enum::Flag2, Enum::Flag3);
    CHECK(flags.bits == 0b0110);
    CHECK_FALSE(util::has_flag(flags, Enum::Flag1));
    CHECK(util::has_flag(flags, Enum::Flag2));
    CHECK(util::has_flag(flags, Enum::Flag3));

    flags = util::make_flags(Enum::Flag3, Enum::Flag1);
    CHECK(flags.bits == 0b0101);
    CHECK(util::has_flag(flags, Enum::Flag1));
    CHECK_FALSE(util::has_flag(flags, Enum::Flag2));
    CHECK(util::has_flag(flags, Enum::Flag3));

    flags = util::make_flags(Enum::Flag1, Enum::Flag2, Enum::Flag3);
    CHECK(flags.bits == 0b0111);
    CHECK(util::has_flag(flags, Enum::Flag1));
    CHECK(util::has_flag(flags, Enum::Flag2));
    CHECK(util::has_flag(flags, Enum::Flag3));

    flags = util::all_flags<Enum>();
    CHECK(flags.bits == 0xFFFF);
    CHECK(util::has_flag(flags, Enum::Flag1));
    CHECK(util::has_flag(flags, Enum::Flag2));
    CHECK(util::has_flag(flags, Enum::Flag3));
}

TEST_CASE("[ltb][util][enum_bits] toggle enum flags") {
    enum Enum {
        Flag1,
        Flag2,
        Flag3,
    };

    auto flags = util::Flags<Enum>{};
    CHECK(flags.bits == 0);

    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0b0001);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0b0010);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0b0100);

    flags = Enum::Flag1;
    CHECK(flags.bits == 0b0001);
    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0b0000);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0b0011);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0b0101);

    flags = Enum::Flag2;
    CHECK(flags.bits == 0b0010);
    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0b0011);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0b0000);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0b0110);

    flags = Enum::Flag3;
    CHECK(flags.bits == 0b0100);
    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0b0101);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0b0110);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0b0000);

    flags = util::make_flags(Enum::Flag1, Enum::Flag2);
    CHECK(flags.bits == 0b0011);
    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0b0010);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0b0001);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0b0111);

    flags = util::make_flags(Enum::Flag2, Enum::Flag3);
    CHECK(flags.bits == 0b0110);
    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0b0111);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0b0100);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0b0010);

    flags = util::make_flags(Enum::Flag3, Enum::Flag1);
    CHECK(flags.bits == 0b0101);
    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0b0100);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0b0111);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0b0001);

    flags = util::make_flags(Enum::Flag1, Enum::Flag2, Enum::Flag3);
    CHECK(flags.bits == 0b0111);
    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0b0110);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0b0101);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0b0011);

    flags = util::all_flags<Enum>();
    CHECK(flags.bits == 0xFFFFFFFF);
    CHECK(util::toggle_flag(flags, Enum::Flag1).bits == 0xFFFFFFF0 + 0b1110);
    CHECK(util::toggle_flag(flags, Enum::Flag2).bits == 0xFFFFFFF0 + 0b1101);
    CHECK(util::toggle_flag(flags, Enum::Flag3).bits == 0xFFFFFFF0 + 0b1011);
}

TEST_CASE("[ltb][util][enum_bits] add enum flags") {
    enum class Enum : std::uint64_t {
        Flag1,
        Flag2,
        Flag3,
    };

    auto flags = util::Flags<Enum>{};
    CHECK(flags.bits == 0);

    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0b0001);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0b0010);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0b0100);

    flags = Enum::Flag1;
    CHECK(flags.bits == 0b0001);
    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0b0001);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0b0011);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0b0101);

    flags = Enum::Flag2;
    CHECK(flags.bits == 0b0010);
    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0b0011);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0b0010);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0b0110);

    flags = Enum::Flag3;
    CHECK(flags.bits == 0b0100);
    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0b0101);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0b0110);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0b0100);

    flags = util::make_flags(Enum::Flag1, Enum::Flag2);
    CHECK(flags.bits == 0b0011);
    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0b0011);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0b0011);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0b0111);

    flags = util::make_flags(Enum::Flag2, Enum::Flag3);
    CHECK(flags.bits == 0b0110);
    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0b0111);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0b0110);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0b0110);

    flags = util::make_flags(Enum::Flag3, Enum::Flag1);
    CHECK(flags.bits == 0b0101);
    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0b0101);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0b0111);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0b0101);

    flags = util::make_flags(Enum::Flag1, Enum::Flag2, Enum::Flag3);
    CHECK(flags.bits == 0b0111);
    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0b0111);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0b0111);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0b0111);

    flags = util::all_flags<Enum>();
    CHECK(flags.bits == 0xFFFFFFFFFFFFFFFF);
    CHECK(util::add_flag(flags, Enum::Flag1).bits == 0xFFFFFFFFFFFFFFFF);
    CHECK(util::add_flag(flags, Enum::Flag2).bits == 0xFFFFFFFFFFFFFFFF);
    CHECK(util::add_flag(flags, Enum::Flag3).bits == 0xFFFFFFFFFFFFFFFF);
}

TEST_CASE("[ltb][util][enum_bits] remove enum flags") {
    enum class Enum : std::uint8_t {
        Flag1,
        Flag2,
        Flag3,
    };

    auto flags = util::Flags<Enum>{};
    CHECK(flags.bits == 0);

    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b0000);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b0000);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b0000);

    flags = Enum::Flag1;
    CHECK(flags.bits == 0b0001);
    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b0000);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b0001);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b0001);

    flags = Enum::Flag2;
    CHECK(flags.bits == 0b0010);
    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b0010);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b0000);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b0010);

    flags = Enum::Flag3;
    CHECK(flags.bits == 0b0100);
    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b0100);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b0100);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b0000);

    flags = util::make_flags(Enum::Flag1, Enum::Flag2);
    CHECK(flags.bits == 0b0011);
    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b0010);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b0001);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b0011);

    flags = util::make_flags(Enum::Flag2, Enum::Flag3);
    CHECK(flags.bits == 0b0110);
    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b0110);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b0100);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b0010);

    flags = util::make_flags(Enum::Flag3, Enum::Flag1);
    CHECK(flags.bits == 0b0101);
    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b0100);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b0101);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b0001);

    flags = util::make_flags(Enum::Flag1, Enum::Flag2, Enum::Flag3);
    CHECK(flags.bits == 0b0111);
    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b0110);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b0101);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b0011);

    flags = util::all_flags<Enum>();
    CHECK(flags.bits == 0xFF);
    CHECK(util::remove_flag(flags, Enum::Flag1).bits == 0b11111110);
    CHECK(util::remove_flag(flags, Enum::Flag2).bits == 0b11111101);
    CHECK(util::remove_flag(flags, Enum::Flag3).bits == 0b11111011);
}

namespace test {

enum UnscopedEnum : std::uint8_t {
    Flag1,
    Flag2,
    Flag3,
    Flag4,
    Flag5,
};

enum class ScopedEnum : std::uint8_t {
    Flag1,
    Flag2,
    Flag3,
    Flag4,
    Flag5,
};

} // namespace test

TEST_CASE_TEMPLATE("[ltb][util][enum_bits] flag_operators", Enum, test::UnscopedEnum, test::ScopedEnum) {
    using namespace util::flag_operators;

    CHECK(util::Flags<Enum>{}.bits == 0);

    SUBCASE("~") {
        CHECK(~util::Flags<Enum>{} == util::Flags<Enum>{0b11111111});
        CHECK(~Enum::Flag3 == util::Flags<Enum>{0b11111011});
        CHECK(~Enum::Flag4 == util::Flags<Enum>{0b11110111});
        CHECK(~(Enum::Flag1 | Enum::Flag2 | Enum::Flag5) == util::Flags<Enum>{0b11101100});
    }

    SUBCASE("|") {
        CHECK((util::Flags{Enum::Flag5} | util::Flags{Enum::Flag2}) == util::Flags<Enum>{0b00010010});
        CHECK((Enum::Flag3 | util::Flags{Enum::Flag1}) == util::Flags<Enum>{0b00000101});
        CHECK((util::Flags{Enum::Flag2} | Enum::Flag4) == util::Flags<Enum>{0b00001010});
        CHECK((Enum::Flag4 | Enum::Flag3 | Enum::Flag5) == util::Flags<Enum>{0b00011100});
    }

    SUBCASE("&") {
        auto odds  = Enum::Flag1 | Enum::Flag3 | Enum::Flag5;
        auto evens = Enum::Flag2 | Enum::Flag4;
        auto high  = Enum::Flag4 | Enum::Flag5;

        CHECK((util::Flags{Enum::Flag5} & util::Flags{Enum::Flag5}) == util::Flags<Enum>{0b00010000});
        CHECK((util::Flags{Enum::Flag5} & util::Flags{Enum::Flag4}) == util::Flags<Enum>{0b00000000});

        CHECK((Enum::Flag1 & odds) == util::Flags<Enum>{0b00000001});
        CHECK((Enum::Flag1 & evens) == util::Flags<Enum>{0b00000000});

        CHECK((odds & Enum::Flag4) == util::Flags<Enum>{0b00000000});
        CHECK((evens & Enum::Flag4) == util::Flags<Enum>{0b00001000});

        CHECK((Enum::Flag2 & Enum::Flag2) == util::Flags<Enum>{0b00000010});
        CHECK((Enum::Flag1 & Enum::Flag2) == util::Flags<Enum>{0b00000000});

        CHECK((odds & evens) == util::Flags<Enum>{0b00000000});
        CHECK((odds & high) == util::Flags<Enum>{0b00010000});
        CHECK((high & evens) == util::Flags<Enum>{0b00001000});
    }

    SUBCASE("^") {
        auto odds  = Enum::Flag1 | Enum::Flag3 | Enum::Flag5;
        auto evens = Enum::Flag2 | Enum::Flag4;
        auto high  = Enum::Flag4 | Enum::Flag5;

        CHECK((util::Flags{Enum::Flag5} ^ util::Flags{Enum::Flag5}) == util::Flags<Enum>{0b00000000});
        CHECK((util::Flags{Enum::Flag5} ^ util::Flags{Enum::Flag4}) == util::Flags<Enum>{0b00011000});

        CHECK((Enum::Flag1 ^ odds) == util::Flags<Enum>{0b00010100});
        CHECK((Enum::Flag1 ^ evens) == util::Flags<Enum>{0b00001011});

        CHECK((odds ^ Enum::Flag4) == util::Flags<Enum>{0b00011101});
        CHECK((evens ^ Enum::Flag4) == util::Flags<Enum>{0b00000010});

        CHECK((Enum::Flag2 ^ Enum::Flag2) == util::Flags<Enum>{0b00000000});
        CHECK((Enum::Flag1 ^ Enum::Flag2) == util::Flags<Enum>{0b00000011});

        CHECK((odds ^ evens) == util::Flags<Enum>{0b00011111});
        CHECK((odds ^ high) == util::Flags<Enum>{0b00001101});
        CHECK((high ^ evens) == util::Flags<Enum>{0b00010010});
    }

    SUBCASE("|=") {
        auto flags = util::Flags<Enum>{0b00010010};
        flags |= util::Flags<Enum>{0b00011001};
        CHECK(flags.bits == 0b00011011);
        flags |= Enum::Flag3;
        CHECK(flags.bits == 0b00011111);
    }

    SUBCASE("&=") {
        auto flags = util::Flags<Enum>{0b00011011};
        flags &= util::Flags<Enum>{0b00011001};
        CHECK(flags.bits == 0b00011001);
        flags &= Enum::Flag4;
        CHECK(flags.bits == 0b00001000);
    }

    SUBCASE("^=") {
        auto flags = util::Flags<Enum>{0b00001101};
        flags ^= util::Flags<Enum>{0b00011001};
        CHECK(flags.bits == 0b00010100);
        flags ^= Enum::Flag2;
        CHECK(flags.bits == 0b00010110);
    }
}

} // namespace
