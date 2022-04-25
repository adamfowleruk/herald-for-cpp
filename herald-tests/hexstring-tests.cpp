//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "catch.hpp"

#include "herald/herald.h"

TEST_CASE("datatypes-hexstring-empty", "[datatypes][hexstring][empty]") {
  SECTION("datatypes-hexstring-empty") {
    herald::data::String hex("");
    herald::datatype::HexString base16;
    bool ok = herald::datatype::HexString::from("",base16);
    
    REQUIRE(ok);
    REQUIRE(hex == base16.encoded());
  }
}

TEST_CASE("datatypes-hexstring-single-zero", "[datatypes][hexstring][single]") {
  SECTION("datatypes-hexstring-single-zero") {
    herald::data::String hex("00");
    herald::datatype::HexString base16;
    bool ok = herald::datatype::HexString::from("00", base16);

    REQUIRE(ok);
    REQUIRE(hex == base16.encoded());
  }
}

TEST_CASE("datatypes-hexstring-expected", "[datatypes][hexstring][expected]") {
  SECTION("datatypes-hexstring-expected") {
    herald::data::String hex(
        "6e340b9cffb37a989ca544e6bb780a2c78901d3fb33738768511a30617afa01d");
    herald::datatype::HexString base16;
    bool ok = herald::datatype::HexString::from(
        "6e340b9cffb37a989ca544e6bb780a2c78901d3fb33738768511a30617afa01d",
        base16);

    REQUIRE(ok);
    REQUIRE(hex == base16.encoded());
  }
}