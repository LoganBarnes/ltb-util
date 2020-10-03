// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Networking
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#include "async_client.hpp"

// external
#include <grpc++/create_channel.h>

namespace ltb::net {

namespace {

auto to_client_connection_state(grpc_connectivity_state const& state) -> ClientConnectionState {
    switch (state) {
    case GRPC_CHANNEL_IDLE:
        return ClientConnectionState::NotConnected;
    case GRPC_CHANNEL_CONNECTING:
        return ClientConnectionState::AttemptingToConnect;
    case GRPC_CHANNEL_READY:
        return ClientConnectionState::Connected;
    case GRPC_CHANNEL_TRANSIENT_FAILURE:
        return ClientConnectionState::RecoveringFromFailure;
    case GRPC_CHANNEL_SHUTDOWN:
        return ClientConnectionState::Shutdown;
    }

    throw std::invalid_argument("Invalid grpc_connectivity_state");
}

} // namespace

AsyncClient::AsyncClient(std::string const& host_address) {
    std::lock_guard channel_lock(channel_mutex_);
    data_.channel = grpc::CreateChannel(host_address, grpc::InsecureChannelCredentials());
}

auto AsyncClient::run() -> void {
    {
        std::lock_guard channel_lock(channel_mutex_);
        auto grpc_state = data_.channel->GetState(true);
        data_.connection_state = to_client_connection_state(grpc_state);

        if (data_.state_change_callback) {
            data_.state_change_callback(data_.connection_state);
        }

        // Ask the channel to notify us when state changes by updating 'completion_queue'
        data_.channel->NotifyOnStateChange(grpc_state,
                                           std::chrono::time_point<std::chrono::system_clock>::max(),
                                           &completion_queue_,
                                           data_.tagger.make_tag(nullptr, ClientTagLabel::ConnectionChange));
    }
    //);

    void* raw_tag;
    bool completed_successfully;

    while (completion_queue_.Next(&raw_tag, &completed_successfully)) {
        std::lock_guard channel_lock(channel_mutex_);
        auto tag = data_.tagger.get_tag(raw_tag);
        // std::cout << (completed_successfully ? "Success: " : "Failure: ") << tag << std::endl;

        switch (tag.label) {

        case ClientTagLabel::ConnectionChange: {
            if (completed_successfully && data_.channel) {
                auto grpc_state = data_.channel->GetState(true);

                auto state = to_client_connection_state(grpc_state);

                if (data_.connection_state != state && data_.state_change_callback) {
                    data_.state_change_callback(state);
                }
                data_.connection_state = state;

                // Ask the channel to notify us when state changes by updating 'completion_queue_'
                data_.channel->NotifyOnStateChange(grpc_state,
                                                   std::chrono::time_point<std::chrono::system_clock>::max(),
                                                   &completion_queue_,
                                                   data_.tagger.make_tag(nullptr, ClientTagLabel::ConnectionChange));
            }

        } break;

        case ClientTagLabel::Finished: {
        } break;

        } // end switch
    }
}

auto AsyncClient::on_state_change(StateChangeCallback callback) -> AsyncClient& {
    std::lock_guard channel_lock(channel_mutex_);
    data_.state_change_callback = callback;
    return *this;
}

auto AsyncClient::shutdown() -> void {
    std::lock_guard channel_lock(channel_mutex_);
    completion_queue_.Shutdown();
    data_.channel = nullptr;
}

} // namespace ltb::net
