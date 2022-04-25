//  Copyright 2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/data/payload_data_formatter.h"
#include "herald/datatype/payload_data.h"
#include "herald/data/string_utils.h"

namespace herald::data {

using namespace herald::datatype;

herald::data::String
ConcretePayloadDataFormatter::shortFormat(const PayloadData& payloadData) const noexcept
{
  return payloadData.shortName();
}

}