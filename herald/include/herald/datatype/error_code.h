//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_ERROR_CODE_H
#define HERALD_ERROR_CODE_H

#include "herald/data/string_utils.h"

namespace herald {
namespace datatype {

// Concept of an error code
class ErrorCode {
public:
  ErrorCode() : mSuccess(true), mMessage("") { }
  ErrorCode(bool success) : mSuccess(success), mMessage("") { }
  ErrorCode(bool success, herald::data::String message)
      : mSuccess(success), mMessage(message) {}
  ~ErrorCode() = default;

  bool operator()() {
    return mSuccess;
  }

  herald::data::String message() { return mMessage; }

 private:
  bool mSuccess;
  herald::data::String mMessage;
};

} // end namespace
} // end namespace

#endif