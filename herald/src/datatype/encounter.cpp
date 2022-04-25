//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/datatype/encounter.h"

#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

// ENCOUNTER DECLARATIONS

Encounter::Encounter(Proximity didMeasure, PayloadData withPayload, Date timestamp)
  : date(timestamp), prox(didMeasure), payloadData(withPayload), valid(true)
{
  ;
}

Encounter::Encounter(Proximity didMeasure, PayloadData withPayload)
  : date(Date()), prox(didMeasure), payloadData(withPayload), valid(true)
{
  ;
}

Encounter::Encounter(const herald::data::String csvRow)
  : date(), prox(), payloadData(), valid(false)
{
  ;
  // TODO parse the csv
}

Encounter::~Encounter() = default;


herald::data::String
Encounter::csvString() const {
  return ""; // TODO fill this out properly
}

bool
Encounter::isValid() const {
  return valid;
}

const Proximity&
Encounter::proximity() const
{
  return prox;
}

const PayloadData&
Encounter::payload() const
{
  return payloadData;
}

const Date&
Encounter::timestamp() const
{
  return date;
}

} // end namespace
} // end namespace
