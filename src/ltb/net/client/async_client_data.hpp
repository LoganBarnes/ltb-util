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
#pragma once

// project
#include "ltb/net/tagger.hpp"
#include "ltb/util/atomic_data.hpp"

// external
#include <grpc++/channel.h>
#include <grpc++/server.h>

// standard
#include <functional>

namespace ltb::net {

enum class ClientConnectionState {
    NoHostSpecified,
    InterprocessServerAlwaysConnected,
    NotConnected,
    AttemptingToConnect,
    Connected,
    RecoveringFromFailure,
    Shutdown,
};

enum class CallImmediately {
    Yes,
    No,
};

struct AsyncClientRpcCallData {
    virtual ~AsyncClientRpcCallData() = default;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // Storage for the status of the RPC upon completion.
    grpc::Status status;
};

template <typename Response>
struct AsyncClientUnaryCallData : public AsyncClientRpcCallData {
    ~AsyncClientUnaryCallData() override = default;

    // Container for the data we expect from the server.
    Response reply;

    std::unique_ptr<grpc_impl::ClientAsyncResponseReader<Response>> response_reader;
};

} // namespace ltb::net
