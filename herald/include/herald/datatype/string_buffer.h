//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_STRING_BUFFER_H
#define HERALD_STRING_BUFFER_H

#include <string>
#include "data.h"

namespace herald {
namespace datatype {

template <std::size_t MaxSize>
class StringBuffer {
public:
  static constexpr std::size_t max_size = MaxSize;

  StringBuffer() noexcept : buffer() {
    buffer.reserve(max_size);
  }

  ~StringBuffer() noexcept = default;

  std::size_t size() const noexcept {
    return buffer.size();
  }

  template <typename T>
  StringBuffer& operator<<(const T& from) noexcept {
    buffer += std::string(from); // explicit conversion
    return *this;
  }

  StringBuffer& operator<<(const char from) noexcept {
    buffer += from;
    return *this;
  }

  template <typename DataRefT>
  StringBuffer& operator<<(const DataRef<DataRefT>& from) noexcept {
    buffer += from.hexEncodedString();
    return *this;
  }

  StringBuffer& operator<<(const std::string& from) noexcept {
    if (buffer.size() + from.size() < max_size) {
      buffer += from;
    }
    // TODO handle partial copy of string that is longer

    // for (const auto& c: from) {
    //   if (used < max_size - 1) { // last saved for termination character
    //     buffer[used++] = c;
    //   }
    // }
    return *this;
  }

  StringBuffer& operator<<(const std::uint8_t& from) noexcept {
    buffer += std::to_string(from);
    return *this;
  }

  StringBuffer& operator<<(const std::uint16_t& from) noexcept {
    buffer += std::to_string(from);
    return *this;
  }

  StringBuffer& operator<<(const std::uint32_t& from) noexcept {
    buffer += std::to_string(from);
    return *this;
  }

  StringBuffer& operator<<(const std::uint64_t& from) noexcept {
    buffer += std::to_string(from);
    return *this;
  }

  StringBuffer& operator<<(const std::int8_t& from) noexcept {
    buffer += std::to_string(from);
    return *this;
  }

  StringBuffer& operator<<(const std::int16_t& from) noexcept {
    buffer += std::to_string(from);
    return *this;
  }

  StringBuffer& operator<<(const std::int32_t& from) noexcept {
    buffer += std::to_string(from);
    return *this;
  }

  StringBuffer& operator<<(const std::int64_t& from) noexcept {
    buffer += std::to_string(from);
    return *this;
  }

  // NOTE: Same as std::uint64_t, so not declared
  // StringBuffer& operator<<(const std::size_t& from) noexcept {
  //   buffer += std::to_string(from);
  // }

  StringBuffer& endl() noexcept {
    if (buffer.size() < max_size - 1) {
      buffer += "\n"; // equivalent of std::endl or std::ends
    }
    return *this;
  }

  StringBuffer& ends() noexcept {
    if (buffer.size() < max_size - 1) {
      buffer += "\0"; // equivalent of std::endl or std::ends
    }
    return *this;
  }

  std::string str() const noexcept {
    return buffer;
  }
private:
  std::basic_string<char> buffer;
};

}
}

#endif