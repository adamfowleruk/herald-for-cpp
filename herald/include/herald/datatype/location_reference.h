//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_LOCATION_REFERENCE_H
#define HERALD_LOCATION_REFERENCE_H

#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

struct LocationReference {
  virtual herald::data::String description() = 0;
};


} // end namespace
} // end namespace

#endif