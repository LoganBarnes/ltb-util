// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace ltb::util {

template <typename Init, typename Destroy, typename... Args>
class GenericGuard;

/**
 * @brief The only way to create a GenericGuard since it has a private constructor
 */
template <typename Init, typename Destroy, typename... Args>
auto make_guard(Init init_func, Destroy destroy_func, Args&&... args) -> GenericGuard<Init, Destroy, Args...>;

/**
 * @brief Serves as a generic way to handle push/pop style calls using RAII
 *
 * This class creates a guard that calls the init function on construction and the destroy
 * function on destruction:
 *
 *     {
 *         auto guard = ltb::util::make_guard(&push, &pop); // calls push() here on creation
 *         ...
 *         // other code
 *         ...
 *     } // calls pop() here when the guard goes out of scope
 *
 * If both functions accept the same arguments or one function takes arguments and the other does not,
 * the function pointers can be used directly and the arguments can be passed in after:
 *
 *     auto guard1 = ltb::util::make_guard(&needs_a_string, &needs_the_same_string, "a string for both functions");
 *     ...
 *     auto guard2 = ltb::util::make_guard(&needs_double, &needs_nothing, 3.14158265359);
 *     ...
 *     auto guard3 = ltb::util::make_guard(&needs_nothing, &needs_two_args, 42, "this is cool");
 *
 * If both functions need differing arguments then lambdas can be used:
 *
 *     auto guard4 = ltb::util::make_guard([] { push(arg1, arg2); }, [] { pop(arg3); );
 *     ...
 *     auto guard5 = ltb::util::make_guard([] { push(arg1, arg2); }, &pop, arg3);
 *
 * @tparam Init is the function called on creation
 * @tparam Destroy is the function called on destruction
 * @tparam Args are the optional arguments used by the Init and Destroy functions
 */
template <typename Init, typename Destroy, typename... Args>
class GenericGuard {
public:
    ~GenericGuard() { call_func(0, destroy_func_, std::make_index_sequence<sizeof...(Args)>()); }

    GenericGuard(GenericGuard const&)          = delete;
    GenericGuard(GenericGuard&& that) noexcept = delete;
    auto operator=(GenericGuard const&) -> GenericGuard& = delete;
    auto operator=(GenericGuard&&) noexcept -> GenericGuard& = delete;

private:
    Destroy                                        destroy_func_;
    std::tuple<typename std::decay<Args>::type...> arguments_;

    /*
     * GenericGuard() is private so this friend function is the only way to create a GenericGuard
     */
    friend auto make_guard<Init, Destroy, Args...>(Init init_func, Destroy destroy_func, Args&&... args)
        -> GenericGuard;

    explicit GenericGuard(Init init_func, Destroy destroy_func, Args&&... args)
        : destroy_func_(destroy_func), arguments_(std::make_tuple(std::forward<Args>(args)...)) {
        call_func(0, init_func, std::make_index_sequence<sizeof...(Args)>());
    }

    // SFINAE function that is called when Func requires arguments
    template <typename Func, std::size_t... S>
    auto call_func(int, Func const& func, std::index_sequence<S...>)
        -> decltype(func(std::get<S>(arguments_)...), void()) {
        func(std::get<S>(arguments_)...);
    }

    // SFINAE function that is called when Func does not require arguments
    template <typename Func, std::size_t... S>
    auto call_func(long, Func const& func, std::index_sequence<S...>) -> decltype(func(), void()) {
        func();
    }
};

template <typename Init, typename Destroy, typename... Args>
auto make_guard(Init init_func, Destroy destroy_func, Args&&... args) -> GenericGuard<Init, Destroy, Args...> {
    return GenericGuard<Init, Destroy, Args...>(std::move(init_func),
                                                std::move(destroy_func),
                                                std::forward<Args>(args)...);
}

} // namespace ltb::util
