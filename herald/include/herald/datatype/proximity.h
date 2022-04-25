//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_PROXIMITY_H
#define HERALD_PROXIMITY_H

#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

enum class ProximityMeasurementUnit : short {
  RSSI, RTT
};

struct Proximity {
  ProximityMeasurementUnit unit;
  double value;

  herald::data::String description() const noexcept {
    return std::to_string((short)unit) + ":" + herald::data::to_string((int)value);
  }

  operator herald::data::String() const noexcept {
    return description();
  }
};


} // end namespace
} // end namespace

#endif