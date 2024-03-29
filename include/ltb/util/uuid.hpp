// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// external
#include <boost/lexical_cast.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

// standard
#include <random>
#include <string>
#include <variant>

namespace ltb::util {

template <typename T>
class Uuid {
public:
    using Value = boost::uuids::uuid;

    constexpr explicit Uuid(Value value) noexcept;

    [[nodiscard]] auto is_nil() const -> bool;

    [[nodiscard]] auto to_string() const -> std::string;

    [[nodiscard]] auto raw_id() const -> Value const&;

    /// \returns the nil uuid.
    static auto nil_id() -> Uuid;

    /// \throws std::invalid_argument if `str` is not a valid uuid.
    static auto from_string(std::string const& str) -> Uuid;

private:
    Value value_;
};

template <typename T>
auto operator==(Uuid<T> const& lhs, Uuid<T> const& rhs);

template <typename T>
auto operator<(Uuid<T> const& lhs, Uuid<T> const& rhs);

template <typename T>
auto hash_id(Uuid<T> const& id) -> std::size_t;

/// \brief
/// \tparam T
template <typename... Ts>
class MultiUuid {
public:
    using Value = std::variant<Uuid<Ts>...>;

    template <typename T>
    MultiUuid(Uuid<T> value); // NOLINT(google-explicit-constructor)

    template <typename... AltTs>
    MultiUuid(MultiUuid<AltTs...> value); // NOLINT(google-explicit-constructor)

    [[nodiscard]] auto is_nil() const -> bool;

    [[nodiscard]] auto to_string() const -> std::string;

    [[nodiscard]] auto raw_id() const -> std::uint32_t const&;

    [[nodiscard]] auto raw_variant() const -> Value const&;

private:
    Value value_;
};

/// \brief
class UuidGenerator {
public:
    explicit UuidGenerator();

    explicit UuidGenerator(std::string const& seed);

    template <typename U>
    auto generate() -> U;

private:
    std::mt19937                                       random_generator_;
    boost::uuids::basic_random_generator<std::mt19937> uuid_generator_;
};

/*
 * Uuid Impl
 */

template <typename T>
constexpr Uuid<T>::Uuid(Value value) noexcept : value_(value) {}

template <typename T>
auto Uuid<T>::is_nil() const -> bool {
    return value_.is_nil();
}

template <typename T>
auto Uuid<T>::to_string() const -> std::string {
    return boost::uuids::to_string(value_);
}

template <typename T>
auto Uuid<T>::raw_id() const -> Value const& {
    return value_;
}

template <typename T>
auto Uuid<T>::nil_id() -> Uuid<T> {
    return Uuid<T>{boost::uuids::nil_generator{}()};
}

template <typename T>
auto Uuid<T>::from_string(std::string const& str) -> Uuid<T> {
    return Uuid<T>{boost::lexical_cast<typename Uuid<T>::Value>(str)};
}

template <typename T>
auto operator==(Uuid<T> const& lhs, Uuid<T> const& rhs) {
    return lhs.raw_id() == rhs.raw_id();
}

template <typename T>
auto operator<(Uuid<T> const& lhs, Uuid<T> const& rhs) {
    return lhs.raw_id() < rhs.raw_id();
}

template <typename T>
auto hash_id(Uuid<T> const& id) -> std::size_t {
    return boost::uuids::hash_value(id.raw_id());
}

/*
 * MultiUuid Impl
 */

template <typename... Ts>
template <typename T>
MultiUuid<Ts...>::MultiUuid(Uuid<T> value) : value_(value) {}

template <typename... Ts>
template <typename... AltTs>
MultiUuid<Ts...>::MultiUuid(MultiUuid<AltTs...> multi_id)
    : MultiUuid<Ts...>(std::visit([](auto id) { return MultiUuid<Ts...>{id}; }, multi_id.raw_variant())) {}

template <typename... Ts>
auto MultiUuid<Ts...>::is_nil() const -> bool {
    return std::visit([](auto const& id) { return id.is_nil(); }, value_);
}

template <typename... Ts>
auto MultiUuid<Ts...>::to_string() const -> std::string {
    return std::visit([](auto const& id) { return id.to_string(); }, value_);
}

template <typename... Ts>
auto MultiUuid<Ts...>::raw_id() const -> std::uint32_t const& {
    return std::visit([](auto const& id) -> std::uint32_t const& { return id.raw_id(); }, value_);
}

template <typename... Ts>
auto MultiUuid<Ts...>::raw_variant() const -> Value const& {
    return value_;
}

template <typename... Ts>
auto operator==(MultiUuid<Ts...> const& lhs, MultiUuid<Ts...> const& rhs) {
    return std::visit([](auto const& l, auto const& r) { return l == r; }, lhs.raw_variant(), rhs.raw_variant());
}

template <typename... Ts>
auto operator<(MultiUuid<Ts...> const& lhs, MultiUuid<Ts...> const& rhs) {
    return std::visit([](auto const& l, auto const& r) { return l < r; }, lhs.raw_variant(), rhs.raw_variant());
}

/*
 * UuidGenerator Impl
 */

template <typename U>
auto UuidGenerator::generate() -> U {
    return U{uuid_generator_()};
}

} // namespace ltb::util

namespace std {

template <typename T>
struct hash<ltb::util::Uuid<T>> {
    auto operator()(ltb::util::Uuid<T> const& id) const -> size_t { return ltb::util::hash_id(id); }
};

template <typename T>
struct hash<ltb::util::MultiUuid<T>> {
    auto operator()(ltb::util::Uuid<T> const& id) const -> size_t {
        return std::visit([](auto const& i) { return ltb::util::hash_id(i); }, id);
    }
};

} // namespace std
