//  Copyright 2021-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/datatype/rssi_minute.h"

#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

RSSIMinute::RSSIMinute()
 : value(0.0)
{
  ;
}

RSSIMinute::RSSIMinute(double value)
 : value(value)
{
  ;
}

RSSIMinute::RSSIMinute(const RSSIMinute& other)
 : value(other.value)
{
  ;
}

RSSIMinute::RSSIMinute(RSSIMinute&& other)
 : value(other.value)
{
  ;
}

RSSIMinute::~RSSIMinute() {}

RSSIMinute&
RSSIMinute::operator=(const RSSIMinute& other)
{
  value = other.value;
  return *this;
}

RSSIMinute&
RSSIMinute::operator=(RSSIMinute&& other)
{
  value = other.value;
  return *this;
}

std::size_t
RSSIMinute::hashCode() const noexcept {
  return std::hash<double>{}(value);
}

RSSIMinute::operator herald::data::String() const noexcept {
  return herald::data::String("RSSIMinute{value=") + 
         herald::data::to_string(value) +
         herald::data::String("}");
}

RSSIMinute::operator double() const noexcept {
  return value;
}



bool
RSSIMinute::operator==(const double other) const noexcept
{
  return value == other;
}

bool
RSSIMinute::operator!=(const double other) const noexcept
{
  return value != other;
}

bool
RSSIMinute::operator==(const RSSIMinute& other) const noexcept
{
  return value == other.value;
}

bool
RSSIMinute::operator!=(const RSSIMinute& other) const noexcept
{
  return value != other.value;
}

bool
RSSIMinute::operator<(const RSSIMinute& other) const noexcept
{
  return value < other.value;
}

bool
RSSIMinute::operator<=(const RSSIMinute& other) const noexcept
{
  return value <= other.value;
}

bool
RSSIMinute::operator>(const RSSIMinute& other) const noexcept
{
  return value > other.value;
}

bool
RSSIMinute::operator>=(const RSSIMinute& other) const noexcept
{
  return value >= other.value;
}





} // end namespace
} // end namespace
