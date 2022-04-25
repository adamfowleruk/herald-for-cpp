//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_PLACENAME_LOCATION_REFERENCE_H
#define HERALD_PLACENAME_LOCATION_REFERENCE_H

#include "location_reference.h"
#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

struct PlacenameLocationReference : public LocationReference {
  PlacenameLocationReference(herald::data::String n)
    : LocationReference(),
      name(n)
  { };
  ~PlacenameLocationReference() = default;

  herald::data::String name;

  herald::data::String description() {
    return "PLACE(name=" + name + ")";
  }
};


} // end namespace
} // end namespace

#endif