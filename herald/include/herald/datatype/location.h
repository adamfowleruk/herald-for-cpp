//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_LOCATION_H
#define HERALD_LOCATION_H

#include "location_reference.h"
#include "date.h"
#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

template <typename LocationReferenceT>
class Location {
public:
  Location(LocationReferenceT&& value, Date&& start, Date&& end)
    : mValue(std::move(value)),mStart(std::move(start)),mEnd(std::move(end))
  {};
  ~Location();

  herald::data::String description() const {
    return mValue->description() + ":[from=" + 
      ((herald::data::String)mStart) + ",to=" + 
      ((herald::data::String)mEnd) + "]";
  }

  operator herald::data::String() const noexcept {
    return description();
  }

private:
  LocationReferenceT mValue;
  Date mStart;
  Date mEnd;
};

} // end namespace
} // end namespace

#endif