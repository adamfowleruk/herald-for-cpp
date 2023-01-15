//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "catch.hpp"
#include "herald/herald.h"

#include <google/protobuf/arena.h>

using namespace herald::mesh;
using namespace herald::mesh::messages;
using namespace herald::datatype;

//   [Who] As a system administrator
//  [What] I need to issue commands to MESH devices from a central location
// [Value] In order to easily administer a large mesh device network

TEST_CASE("mesh-hello-command", "[mesh][command][hello]") {
  SECTION("mesh-hello-command") {
    // Test default hello command execution
    google::protobuf::Arena arena;
    // Create hello message
    Hello* hello = google::protobuf::Arena::CreateMessage<Hello>(&arena);
    hello->set_commandid(1234);
    // Serialise to binary
    std::string output;
    bool outOk = hello->SerializeToString(&output);
    REQUIRE(true == outOk);
    // Deserialise from binary
    Hello* helloIn = google::protobuf::Arena::CreateMessage<Hello>(&arena);
    bool inOk = helloIn->ParseFromString(output);
    REQUIRE(true == inOk);
    // Ensure new message is the same
    REQUIRE(1234 == hello->commandid());
    REQUIRE(1234 == helloIn->commandid());
  }
}