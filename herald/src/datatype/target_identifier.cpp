//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/datatype/target_identifier.h"

#include "herald/datatype/data.h"
#include "herald/datatype/hex_string.h"
#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

TargetIdentifier::TargetIdentifier()
 : value()
{
  ; // TODO set value to random v4 UUID string
}

TargetIdentifier::TargetIdentifier(const Data& data)
  : value(data)
{
  ;
}

TargetIdentifier::TargetIdentifier(const TargetIdentifier& from)
  : value(from.value)
{
  ;
}

TargetIdentifier::~TargetIdentifier() {}

TargetIdentifier&
TargetIdentifier::operator=(const TargetIdentifier& from)
{
  value = from.value;
  return *this;
}

bool
TargetIdentifier::operator==(const TargetIdentifier& other) const noexcept {
  return hashCode() == other.hashCode();
}

bool
TargetIdentifier::operator==(const Data& other) const noexcept {
  return value == other;
}

bool
TargetIdentifier::operator!=(const TargetIdentifier& other) const noexcept {
  return hashCode() != other.hashCode();
}

bool
TargetIdentifier::operator!=(const Data& other) const noexcept {
  return value != other;
}
bool
TargetIdentifier::operator<(const TargetIdentifier& other) const noexcept {
  return value < other.value;
}

bool
TargetIdentifier::operator>(const TargetIdentifier& other) const noexcept {
  return value > other.value;
}

std::size_t
TargetIdentifier::hashCode() const {
  return std::hash<Data>{}(value);
}

TargetIdentifier::operator herald::data::String() const {
  return HexString::encode(value).encoded();
}

TargetIdentifier::operator Data() const {
  return value;
}

Data
TargetIdentifier::underlyingData() const {
  return Data(value);
}

} // end namespace
} // end namespace
