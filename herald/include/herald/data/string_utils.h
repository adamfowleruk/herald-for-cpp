//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_STRING_UTILS_H
#define HERALD_STRING_UTILS_H

#ifndef CONFIG_HERALD_NO_STD_STREAMS
#include <iostream> // for 'operator << char' support

#ifndef CONFIG_HERALD_NO_STD_STRING
#include <ostream>
#include <sstream>
#endif

#endif

#ifndef CONFIG_HERALD_NO_STD_STRING
#include <string>
#else
#include "herald/datatype/data.h"
#endif

namespace herald {
namespace data {

// Define String and StringStream types that differ depending upon libs

#ifndef CONFIG_HERALD_NO_STD_STRING

using String = std::string;
using StringStream = std::stringstream;
#define EOS std::ends

#else

// FWD DECL
class DataStringIterator;

class DataString {
public:
  DataString() noexcept;
  DataString(herald::datatype::Data&& toOwn) noexcept;
  DataString(const char* v) noexcept;
  DataString(DataString&& toMove) noexcept;
  DataString(const DataString& toCopy) noexcept; // Don't delete this - too much std::string code relies on copy construction = delete;
  ~DataString() noexcept;

  using iterator = DataStringIterator;
  static const std::size_t npos = -1; // DELIBERATE cross platform

  /// \brief WARNING MAY BE UNSAFE ON EMBEDDED SYSTEMS - duplicates data in memory
  const char* c_str() const noexcept;
  std::size_t size() const noexcept;
  std::size_t find(char c) const noexcept;
  const char at(std::size_t idx) const noexcept;
  DataString substr(std::size_t fromIdx) const noexcept;
  DataString substr(std::size_t fromIdx, std::size_t length) const noexcept;

  char operator[](std::size_t idx) const noexcept;
  bool operator==(const DataString& other) const noexcept;
  bool operator!=(const DataString& other) const noexcept;
  DataString& operator+=(const DataString& toAppend) noexcept;
  DataString& operator+=(const char toAppend) noexcept;
  /// \brief Creates a new String from this + next string
  /// \note Avoid using this as it's RAM wasteful. Use DataStringStream instead
  DataString operator+(const DataString& toAppend) const noexcept;
  DataString operator+(const char) const noexcept;
  DataString& operator=(const DataString& toCopyAssign) noexcept;
  DataString& operator=(const DataString&& toMoveAssign) noexcept;

  /// \brief Implicit conversion to Data for Data.append() to work
  operator herald::datatype::Data() const noexcept;

  iterator begin() const noexcept;
  iterator end() const noexcept;

private:
  herald::datatype::Data value;
};

class DataStringIterator {
public:
  DataStringIterator(const DataString& stringOver) noexcept;
  DataStringIterator(DataString&& stringOver) noexcept;
  ~DataStringIterator() noexcept;

  char operator*() noexcept;

  DataStringIterator& operator=(const DataStringIterator& consume) noexcept;
  DataStringIterator& operator=(DataStringIterator&& consume) noexcept;

  bool operator==(const DataStringIterator& other) const noexcept;
  bool operator!=(const DataStringIterator& other) const noexcept;
  void operator++() noexcept;
  void operator--() noexcept;

 private:
  std::size_t pos;
  const DataString& over;
};

struct EOSStruct {};

class DataStringStream {
public:
  DataStringStream() noexcept;
  DataStringStream(DataStringStream&& toMove) noexcept;
  DataStringStream(const DataStringStream& toCopy) = delete;
  ~DataStringStream() noexcept;

  DataStringStream& operator<<(DataString consume) noexcept;
  // DataStringStream& operator<<(std::size_t consume) noexcept;
  DataStringStream& operator<<(std::uint8_t consume) noexcept;
  DataStringStream& operator<<(std::uint16_t consume) noexcept;
  DataStringStream& operator<<(std::uint32_t consume) noexcept;
  DataStringStream& operator<<(std::uint64_t consume) noexcept;
  DataStringStream& operator<<(std::int8_t consume) noexcept;
  DataStringStream& operator<<(std::int16_t consume) noexcept;
  DataStringStream& operator<<(std::int32_t consume) noexcept;
  DataStringStream& operator<<(std::int64_t consume) noexcept;
  DataStringStream& operator<<(long consume) noexcept;
  DataStringStream& operator<<(char consume) noexcept;
  DataStringStream& operator<<(EOSStruct consume) noexcept;

  DataString str() noexcept;

  /// \brief Implicit conversion to Data for Data.append() to work
  /// \note DOES NOT conver to HEX. Equivalent of raw char array access
  operator herald::datatype::Data() const noexcept;

 private:
  herald::datatype::Data value;
};

using String = DataString;
using StringStream = DataStringStream;
#define EOS EOSStruct()

#endif

inline String to_string(std::size_t toConvert) noexcept {
  // TODO
  return String();
}

inline String to_string(std::uint32_t toConvert) noexcept {
  // TODO
  return String();
}

inline String to_string(std::uint64_t toConvert) noexcept {
  // TODO
  return String();
}

inline String to_string(std::int64_t toConvert) noexcept {
  // TODO
  return String();
}

inline String to_string(short toConvert) noexcept {
  // TODO
  return String();
}

inline String to_string(int toConvert) noexcept {
  // TODO
  return String();
}

inline String to_string(long toConvert) noexcept {
  // TODO
  return String();
}

inline String to_string(double toConvert) noexcept {
  // TODO
  return String();
}

}
}


#ifndef CONFIG_HERALD_NO_STD_STREAMS
namespace std {

#ifdef CONFIG_HERALD_NO_STD_STRING
/// \brief Streaming support for herald::data::String
/// \note Not defined if String is a std::string
template <>
inline std::ostream&
operator<<(std::ostream& lhs,
           const herald::data::String& rhs) noexcept
{
  for (auto c: rhs) {
    lhs << (char)c;
  }
  return lhs;
}
#endif

/// \brief Stream for all Herald types that have an explicit herald::data::String operator
template <typename HeraldT,
          typename T = typename std::enable_if<false ==
                       std::is_same<HeraldT, char>::value>::type
         >
inline std::ostream&
operator<<(std::ostream& lhs, const HeraldT& rhs) noexcept {
  return lhs << (herald::data::String)rhs;
}
}
#endif

#ifdef CONFIG_HERALD_NO_STD_STRING
namespace std {

template <>
struct hash<herald::data::DataString> {
  size_t operator()(const herald::data::DataString& v) const {
    return std::hash<herald::datatype::Data>()((herald::datatype::Data)v);
  }
};

}
#endif

#endif  // HERALD_STRING_UTILS_H