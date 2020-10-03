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
    data_.use_safely([this, host_address](auto& data) {
        completion_queue_ = std::make_unique<grpc::CompletionQueue>();
        data.channel = grpc::CreateChannel(host_address, grpc::InsecureChannelCredentials());
    });
}

auto AsyncClient::run() -> void {
    data_.use_safely([this](auto& data) {
        auto grpc_state = data.channel->GetState(true);
        data.connection_state = to_client_connection_state(grpc_state);

        if (data.state_change_callback) {
            data.state_change_callback(data.connection_state);
        }

        // Ask the channel to notify us when state changes by updating 'completion_queue'
        data.channel->NotifyOnStateChange(grpc_state,
                                          grpc::g_core_codegen_interface->gpr_inf_future(GPR_CLOCK_REALTIME),
                                          completion_queue_.get(),
                                          data.tagger.make_tag(nullptr, ClientTagLabel::ConnectionChange));
    });

    void* raw_tag;
    bool completed_successfully;

    while (completion_queue_->Next(&raw_tag, &completed_successfully)) {
        data_.use_safely([this, raw_tag, completed_successfully](auto& data) {
            auto tag = data.tagger.get_tag(raw_tag);
            // std::cout << (completed_successfully ? "Success: " : "Failure: ") << tag << std::endl;

            switch (tag.label) {

            case ClientTagLabel::ConnectionChange: {
                if (completed_successfully && data.channel) {
                    auto grpc_state = data.channel->GetState(true);

                    auto state = to_client_connection_state(grpc_state);

                    if (data.connection_state != state && data.state_change_callback) {
                        data.state_change_callback(state);
                    }
                    data.connection_state = state;

                    // Ask the channel to notify us when state changes by updating 'completion_queue_'
                    data.channel->NotifyOnStateChange(grpc_state,
                                                      grpc::g_core_codegen_interface->gpr_inf_future(
                                                          GPR_CLOCK_REALTIME),
                                                      completion_queue_.get(),
                                                      data.tagger.make_tag(nullptr, ClientTagLabel::ConnectionChange));
                }

            } break;

            case ClientTagLabel::Finished: {
            } break;

            } // end switch
        });
    }
}

auto AsyncClient::on_state_change(StateChangeCallback callback) -> AsyncClient& {
    data_.use_safely([callback](auto& data) { data.state_change_callback = callback; });
    return *this;
}

auto AsyncClient::shutdown() -> void {
    data_.use_safely([this](auto& data) {
        completion_queue_->Shutdown();
        data.channel = nullptr;
    });
}

} // namespace ltb::net
