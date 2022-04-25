//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_TARGET_IDENTIFIER_H
#define HERALD_TARGET_IDENTIFIER_H

#include "data.h"
#include "hex_string.h"
#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

class TargetIdentifier {
public:
  TargetIdentifier();
  TargetIdentifier(const Data& data);
  TargetIdentifier(const TargetIdentifier& from); // copy ctor
  TargetIdentifier& operator=(const TargetIdentifier& from); // copy assign
  ~TargetIdentifier();

  bool operator==(const TargetIdentifier& other) const noexcept;
  bool operator==(const Data& other) const noexcept;
  bool operator!=(const TargetIdentifier& other) const noexcept;
  bool operator!=(const Data& other) const noexcept;
  bool operator<(const TargetIdentifier& other) const noexcept; // required for std::less
  bool operator>(const TargetIdentifier& other) const noexcept; // required for std::less

  std::size_t hashCode() const;

  operator herald::data::String() const;

  operator Data() const;

  Data underlyingData() const;

private:
  Data value;

};

} // end namespace
} // end namespace



namespace std {
#ifndef CONFIG_HERALD_NO_STD_STRING
  inline std::ostream& operator<<(std::ostream &os, const herald::datatype::TargetIdentifier& d)
  {
    return os << herald::datatype::HexString::encode(
      d.underlyingData().reversed()
    ).encoded();
  }
#endif

  template<>
  struct hash<herald::datatype::TargetIdentifier>
  {
    size_t operator()(const herald::datatype::TargetIdentifier& v) const
    {
      return v.hashCode();
    }
  };
} // end namespace

#endif