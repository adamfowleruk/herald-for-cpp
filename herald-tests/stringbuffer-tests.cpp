//  Copyright 2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include <string>

#include "catch.hpp"

#include "herald/herald.h"

TEST_CASE("datatypes-stringbuffer-ctor-default", "[datatypes][stringbuffer][ctor][default]") {
  SECTION("datatypes-stringbuffer-ctor-default") {
    herald::datatype::StringBuffer<128> buffer;
    REQUIRE(herald::datatype::StringBuffer<128>::max_size == 128);
    REQUIRE(buffer.size() == 0);
    std::string out = buffer.str();
    REQUIRE(out.size() == 0);
  }
}

TEST_CASE("datatypes-stringbuffer-append-strings", "[datatypes][stringbuffer][append][strings]") {
  SECTION("datatypes-stringbuffer-append-strings") {
    herald::datatype::StringBuffer<128> buffer;
    std::string s1("First");
    std::string s2("Second");
    buffer << s1 << s2;
    REQUIRE(buffer.size() == 11);
    std::string out = buffer.str();
    REQUIRE(out.size() == 11);
    REQUIRE(out.substr(0,5) == s1);
    REQUIRE(out.substr(5,6) == s2);
  }
}

TEST_CASE("datatypes-stringbuffer-append-mixed", "[datatypes][stringbuffer][append][mixed]") {
  SECTION("datatypes-stringbuffer-append-mixed") {
    herald::datatype::StringBuffer<128> buffer;
    std::string s1("First");
    std::uint32_t i2 = 1234;
    std::string s2(std::to_string(i2));
    buffer << s1 << i2;
    REQUIRE(buffer.size() == 9);
    std::string out = buffer.str();
    REQUIRE(out.size() == 9);
    REQUIRE(out.substr(0,5) == s1);
    REQUIRE(out.substr(5,4) == s2);
  }
}