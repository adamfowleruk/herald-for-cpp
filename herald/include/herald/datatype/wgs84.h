//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_WGS84_H
#define HERALD_WGS84_H

#include "location_reference.h"

#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

struct WGS84CircularAreaLocationReference : public LocationReference {
  double latitude;
  double longitude;
  double altitude;
  double radius;

  herald::data::String description() override {
    return "WGS84(lat=" + herald::data::to_string(latitude) + 
      ",lon=" + herald::data::to_string(longitude) + 
      ",alt=" + herald::data::to_string(altitude) + 
      ",radius=" + herald::data::to_string(radius) + ")";
  }
};



struct WGS84PointLocationReference : public LocationReference {
  double latitude;
  double longitude;
  double altitude;

  herald::data::String description() override {
    return "WGS84(lat=" + herald::data::to_string(latitude) + 
      ",lon=" + herald::data::to_string(longitude) + 
      ",alt=" + herald::data::to_string(altitude) + ")";
  }
};


} // end namespace
} // end namespace

#endif