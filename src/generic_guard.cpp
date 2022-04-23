// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/generic_guard.hpp"

// external
#include <doctest/doctest.h>

// standard
#include <string>

namespace {

bool push_called = false;
bool pop_called  = false;

[[maybe_unused]] auto void_push() {
    push_called = true;
}
[[maybe_unused]] auto void_pop() {
    pop_called = true;
}

[[maybe_unused]] auto float_push(float) {
    push_called = true;
}
[[maybe_unused]] auto float_pop(float) {
    pop_called = true;
}

[[maybe_unused]] auto string_push(std::string const&) {
    push_called = true;
}
[[maybe_unused]] auto string_pop(std::string const&) {
    pop_called = true;
}

[[maybe_unused]] auto int_and_string_push(int, std::string const&) {
    push_called = true;
}
[[maybe_unused]] auto int_and_string_pop(int, std::string const&) {
    pop_called = true;
}

} // namespace

TEST_CASE("[ltb][util][guard] generic_guard") {
    using namespace ltb;

    push_called = false;
    pop_called  = false;

    SUBCASE("No lambdas") {

        SUBCASE("void push, void pop") {
            auto guard = util::make_guard(void_push, void_pop);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("float push, void pop") {
            auto guard = util::make_guard(float_push, void_pop, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("void push, float pop") {
            auto guard = util::make_guard(void_push, float_pop, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("float push, float pop") {
            auto guard = util::make_guard(float_push, float_pop, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("string push, void pop") {
            auto guard = util::make_guard(string_push, void_pop, "Will this compile?");
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("void push, string pop") {
            auto guard = util::make_guard(void_push, string_pop, "Of course it will compile!");
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("string push, string pop") {
            auto guard = util::make_guard(string_push, string_pop, std::string("This will too!"));
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("int and string push, void pop") {
            auto guard = util::make_guard(int_and_string_push, void_pop, 3, "Multi-arg?");
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("void push, int and string pop") {
            auto guard = util::make_guard(void_push, int_and_string_pop, 7, "");
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("int and string push, int and string pop") {
            auto guard = util::make_guard(int_and_string_push, int_and_string_pop, 21, std::string());
            CHECK(push_called);
            CHECK(!pop_called);
        }
    }

    SUBCASE("With lambdas") {

        SUBCASE("lambda push, void pop") {
            auto guard = util::make_guard([] { void_push(); }, void_pop);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("lambda push, float pop") {
            auto guard = util::make_guard([] { float_push(0.f); }, float_pop, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("void push, lamda pop") {
            auto guard = util::make_guard(void_push, [] { float_pop(0.f); });
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("float push, lambda pop") {
            auto guard = util::make_guard(
                float_push, [] { void_pop(); }, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("lambda push, lambda pop") {
            auto guard = util::make_guard([] { float_push(0.f); }, [] { int_and_string_pop(73, "blarg"); });
            CHECK(push_called);
            CHECK(!pop_called);
        }

        SUBCASE("float lambda push, float lambda pop") {
            auto guard = util::make_guard([](float arg) { float_push(arg); }, [](float arg) { float_pop(arg); }, 0.f);
            CHECK(push_called);
            CHECK(!pop_called);
        }
    }

    CHECK(push_called);
    CHECK(pop_called);
}
