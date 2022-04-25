//  Copyright 2021-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_PAYLOAD_DATA_FORMATTER_H
#define HERALD_PAYLOAD_DATA_FORMATTER_H

#include "../datatype/payload_data.h"

#include "string_utils.h"

namespace herald::data {

using namespace herald::datatype;

class PayloadDataFormatter {
public:
  PayloadDataFormatter() = default;
  virtual ~PayloadDataFormatter() = default;

  virtual herald::data::String shortFormat(const PayloadData& payloadData) const noexcept = 0;
};

class ConcretePayloadDataFormatter : public PayloadDataFormatter {
public:
  ConcretePayloadDataFormatter() = default;
  ~ConcretePayloadDataFormatter() = default;

  herald::data::String shortFormat(const PayloadData& payloadData) const noexcept override;
};

}

#endif