// ///////////////////////////////////////////////////////////////////////////////////////
//                                                                           |________|
//  Copyright (c) 2020 CloudNC Ltd - All Rights Reserved                        |  |
//                                                                              |__|
//        ____                                                                .  ||
//       / __ \                                                               .`~||$$$$
//      | /  \ \         /$$$$$$  /$$                           /$$ /$$   /$$  /$$$$$$$
//      \ \ \ \ \       /$$__  $$| $$                          | $$| $$$ | $$ /$$__  $$
//    / / /  \ \ \     | $$  \__/| $$  /$$$$$$  /$$   /$$  /$$$$$$$| $$$$| $$| $$  \__/
//   / / /    \ \__    | $$      | $$ /$$__  $$| $$  | $$ /$$__  $$| $$ $$ $$| $$
//  / / /      \__ \   | $$      | $$| $$  \ $$| $$  | $$| $$  | $$| $$  $$$$| $$
// | | / ________ \ \  | $$    $$| $$| $$  | $$| $$  | $$| $$  | $$| $$\  $$$| $$    $$
//  \ \_/ ________/ /  |  $$$$$$/| $$|  $$$$$$/|  $$$$$$/|  $$$$$$$| $$ \  $$|  $$$$$$/
//   \___/ ________/    \______/ |__/ \______/  \______/  \_______/|__/  \__/ \______/
//
// ///////////////////////////////////////////////////////////////////////////////////////
#include "example_client.hpp"

// project
#include "ltb/net/client/async_client.hpp"

// generated
#include <protos/chat/chat_room.grpc.pb.h>

// external
#include <grpc++/create_channel.h>

// standard
#include <thread>

namespace ltb::example {

ExampleClient::ExampleClient() = default;

} // namespace ltb::example

namespace {

auto operator<<(std::ostream& os, ltb::net::ClientConnectionState const& state) -> std::ostream& {
    switch (state) {
    case ltb::net::ClientConnectionState::NotConnected:
        return os << "NotConnected";
    case ltb::net::ClientConnectionState::AttemptingToConnect:
        return os << "AttemptingToConnect";
    case ltb::net::ClientConnectionState::Connected:
        return os << "Connected";
    case ltb::net::ClientConnectionState::RecoveringFromFailure:
        return os << "RecoveringFromFailure";
    case ltb::net::ClientConnectionState::Shutdown:
        return os << "Shutdown";
    }
    throw std::invalid_argument("Invalid grpc_connectivity_state type");
}

} // namespace

auto main() -> int {
    const auto host_address = "0.0.0.0:50051";

    // using Service = ltb::example::ChatRoom;

    ltb::net::AsyncClient client(host_address);

    client.on_state_change([](auto state) { std::cout << state << std::endl; });

    std::thread run_thread([&client] { client.run(); });

    std::cin.ignore();
    client.shutdown();

    run_thread.join();

    return 0;
}
