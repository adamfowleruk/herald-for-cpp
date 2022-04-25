//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_PAYLOAD_DATA_H
#define HERALD_PAYLOAD_DATA_H

#include "data.h"
#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

class PayloadData : public Data {
public:
  PayloadData();
  PayloadData(const Data& from);
  PayloadData(const std::byte* data, std::size_t length);
  PayloadData(std::byte repeating, std::size_t count);
  ~PayloadData() = default;
  PayloadData& operator=(const PayloadData& other);

  herald::data::String shortName() const;
  operator herald::data::String() const;
};

} // end namespace
} // end namespace

#endif