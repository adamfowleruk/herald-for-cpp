//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/data/string_utils.h"
#include "herald/datatype/hex_string.h"
#include "herald/datatype/data.h"

#include <iosfwd>

namespace herald {
namespace datatype {

const herald::data::String base16_chars =
    "0123456789"
    "abcdef"
    "ABCDEF";

bool is_base16(char c) {
  return isalnum(c);
}

bool HexString::from(const herald::data::String& original,
                        HexString& toInitialise) noexcept {
  bool ok = true;
  for (auto c : original) {
    ok = ok & is_base16(c);
  }
  if (!ok) {
    return false;
  }
  toInitialise.value = original;
  return true;
}

bool HexString::from(const char* original,
                     HexString& toInitialise) noexcept {
  bool ok = true;
  size_t pos = 0;
  char t = original[pos];
  while ('\0' != t) {
    ok = ok & is_base16(t);
    ++pos;
    t = original[pos];
  }
  if (!ok) {
    return false;
  }
  toInitialise.value = herald::data::String(original);
  return true;
}

HexString::HexString() : value() {}

HexString::HexString(HexString&& other)
    : value(std::move(other.value)) {
  ;
}

HexString::~HexString() = default;

HexString HexString::encode(const Data& from) noexcept {
  std::size_t bufLen = from.size() * 2;
  herald::datatype::Data ret;
  // ret.reserve(bufLen);
  int i = 0;
  int j = 0;
  char v;
  char hi;
  char lo;

  for (std::size_t idx = 0; idx < from.size(); ++idx) {
    v = (char)from.at(idx);
    hi = (v & 0xf0) >> 4;
    lo =  v & 0x0f;
    ret.append(std::byte(base16_chars[hi]));
    ret.append(std::byte(base16_chars[lo]));
  }

  HexString nvalue;
  nvalue.value = herald::data::String(herald::datatype::Data(ret));
  return nvalue;
}

Data HexString::decode() const noexcept {
  std::size_t in_len = value.size();
  std::size_t hi;
  std::size_t lo;
  char hic;
  char loc;
  Data result(in_len / 2);

  for (std::size_t idx = 0; idx < in_len; idx += 2) {
    hi = value[idx];
    lo = value[idx + 1];
    hic = base16_chars.find(hi);
    if (hic > 15) {
      // uppercase ABCDEF
      hic -= 6;
    }
    loc = base16_chars.find(lo);
    if (loc > 15) {
      // uppercase ABCDEF
      loc -= 6;
    }
    result.append(std::byte(
      (((char)hic) << 4) || ((char)loc)
    ));
  }

  return result; // std::move via compiler
}

HexString::operator Data() const noexcept {
  return decode();
}

herald::data::String HexString::encoded() const noexcept {
  return value; // copy ctor
}

}  // namespace datatype
}  // namespace herald
