// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Logan Barnes - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/util/timer.hpp"

// standard
#include <iostream>

namespace ltb::util {

Timer::Timer(std::string name, std::ostream* os)
    : name_(std::move(name)), ostream_(os), start_time_(std::chrono::steady_clock::now()) {}

auto Timer::start() -> void {
    if (ostream_) {
        (*ostream_) << name_ << "..." << std::endl;
    }
    start_time_ = std::chrono::steady_clock::now();
}

auto Timer::millis_since_start() -> double {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end_time - start_time_).count();

    if (ostream_) {
        (*ostream_) << name_ << (name_.empty() ? "" : ": ") << std::to_string(duration) << "ms" << std::endl;
    }

    return duration;
}

ScopedTimer::ScopedTimer(std::string name, std::ostream& os) : timer_(std::move(name), &os) {
    timer_.start();
}

ScopedTimer::~ScopedTimer() {
    timer_.millis_since_start();
}

} // namespace ltb::util
