//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/datatype/uuid.h"

#include "herald/datatype/base64_string.h"
#include "herald/datatype/data.h"
#include "herald/datatype/hex_string.h"
#include "herald/datatype/randomness.h"
#include "herald/data/string_utils.h"

#include <algorithm>
#include <array>
// #include <iomanip>
// #include <iosfwd>
// #include <sstream>
// #include <string>
#include <type_traits>

namespace herald {
namespace datatype {

// Implementation Class

// Static functions
UUID
UUID::fromString(const herald::data::String& from) noexcept {
  // remove hyphens before using hex decoding
  herald::data::String newFrom = from;  // copy
  newFrom.erase(std::remove(newFrom.begin(),newFrom.end(),'-'), newFrom.end());
  HexString hs;
  bool ok = HexString::from(newFrom, hs);
  if (!ok) {
    return UUID(std::array<value_type, 16>{{0}}, false);
  }
  auto dataInstance = hs.encoded();

  std::array<value_type, 16> data{ {0} };
  if (dataInstance.size() != 16) {
    return UUID(data,false);
  }
  for (std::size_t pos = 0;pos < 16;++pos) {
    data[pos] = (value_type)dataInstance.at(pos);
  }
  UUID uuid(data,true); // TODO check UUID is a V4 format
  return uuid;
}

// Instance functions
UUID::UUID(const char* from) noexcept
 : mData({0}), mValid(false)
{
  const int len = strlen(from);
  if (0 == len) {
    return;
  }
  if (32 != len && 36 != len) {
    return; // not a valid UUID
  }
  char newid[33] = {'0'};
  // Check if it has hyphens
  if (36 == len) {
    int p = 0;
    for (; p < 8; ++p) {
      newid[p] = from[p];
    }
    ++p; // past first hyphen
    for (; p < 13; ++p) {
      newid[p - 1] = from[p];
    }
    ++p; // past second hyphen
    for (; p < 18; ++p) {
      newid[p - 2] = from[p];
    }
    ++p; // past third hyphen
    for (; p < 23; ++p) {
      newid[p - 3] = from[p];
    }
    ++p;  // past fourth hyphen
    for (; p < 36; ++p) {
      newid[p - 4] = from[p];
    }
  }
  // Check if it does not have hyphens
  if (32 == len) {
    int p = 0;
    for (; p < 32; ++p) {
      newid[p] = from[p];
    }
  }
  newid[32] = '\0';
  HexString hs;
  bool ok = HexString::from(newid,hs);
  if (!ok) {
    return;
  }
  auto hexData = hs.decode();
  for (int i = 0;i < 16; ++i) {
    mData[i] = (value_type)hexData.at(i);
  }
}

UUID::UUID(UUID&& from) noexcept
 : mData(std::move(from.mData)), mValid(from.mValid)
{
  ;
}

// private ctor
UUID::UUID(std::array<value_type, 16> data, bool isValid) noexcept
 : mData(data),mValid(isValid)
{
  ;
}


UUID&
UUID::operator=(const UUID& other) noexcept
{
  mData = other.mData;
  mValid = other.mValid;
  return *this;
}

bool
UUID::valid() const noexcept {
  return mValid;
}

bool
UUID::operator==(const UUID& other) const noexcept {
  // return mData == other.mData;
  bool eq = true;
  for (std::size_t i = 0;eq && i < max_size;++i) {
    eq = eq && (mData[i] == other.mData[i]);
  }
  return eq;
}
bool
UUID::operator!=(const UUID& other) const noexcept {
  return !(*this == other);
}

bool
UUID::operator<(const UUID& other) const noexcept {
  return mData < other.mData;
}

bool
UUID::operator>(const UUID& other) const noexcept {
  return mData > other.mData;
}

std::array<uint8_t, 16>
UUID::data() const noexcept {
  return mData;
}

UUID::operator herald::data::String() const noexcept {
  // convert bytes to hex string
  // std::stringstream str;
	// str.setf(std::ios_base::hex, std::ios::basefield);
	// str.fill('0');
  // for (std::size_t i=0; i < 16; i++) {
	// 	str << std::setw(2) << (unsigned short)mData[i];
	// }
	// std::string hexString = str.str();
  Data d;
  for (std::size_t i = 0;i < mData.size(); ++i) {
    d.append(mData[i]);
  }
  herald::data::String hexString = HexString::encode(d).encoded();
  // add in hyphens at relevant points
  herald::data::StringStream fstr;
  fstr << hexString.substr(0,8) << "-" << hexString.substr(8,4) << "-"
       << hexString.substr(12,4) << "-" << hexString.substr(16,4) << "-"
       << hexString.substr(20,12);
  return fstr.str();
}

// Static assertions on this classes compiler contract
static_assert(std::is_constructible_v<UUID,const char*>,"UUID Cannot be string constructed");
static_assert(std::is_copy_constructible_v<UUID>,"UUID Cannot be copy constructed");
static_assert(std::is_move_constructible_v<UUID>,"UUID Cannot be move constructed");

} // end namespace
} // end namespace
