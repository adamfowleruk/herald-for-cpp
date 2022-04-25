//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/data/string_utils.h"

#ifdef CONFIG_HERALD_NO_STD_STRING
#include "herald/datatype/data.h"
#endif

namespace herald {
namespace data {

#ifdef CONFIG_HERALD_NO_STD_STRING

DataString::DataString() noexcept
  : value()
{
}

DataString::DataString(herald::datatype::Data&& toOwn) noexcept
  : value(std::move(toOwn))
{
}

DataString::DataString(const char* v) noexcept
  : value(v)
{
}

DataString::DataString(DataString&& toMove) noexcept
  : value(std::move(toMove))
{
}

DataString::DataString(const DataString& toCopy) noexcept
  : value(toCopy)
{
}

DataString::~DataString() noexcept = default;

const char*
DataString::c_str() const noexcept
{
  std::size_t max = size();
  const char* ptr = new char[max + 1];
  for (std::size_t i = 0;i < max; ++i) {
    ptr[i] = (char)value.at[i];
  }
  ptr[max] = '\0';
  return ptr;
}

std::size_t
DataString::size() const noexcept
{
  return value.size();
}

std::size_t
DataString::find(char c) const noexcept
{
  for (std::size_t i = 0;i < size();++i) {
    if ((char)value.at(i) == c) {
      return i;
    }
  }
  return DataString::npos;
}

const char
DataString::at(std::size_t idx) const noexcept
{
  return (const char)value.at(idx);
}

DataString
DataString::substr(std::size_t fromIdx) const noexcept
{
  return DataString(value.subdata(fromIdx));
}

DataString
DataString::substr(std::size_t fromIdx, std::size_t length) const noexcept
{
  return DataString(value.subdata(fromIdx, length));
}

bool
DataString::operator==(const DataString& other) const noexcept
{
  return value == other.value;
}

bool
DataString::operator!=(const DataString& other) const noexcept
{
  return value != other.value;
}

DataString&
DataString::operator+=(const DataString& toAppend) noexcept
{
  value.append(toAppend.value);
}

DataString
DataString::operator+(const DataString& toAppend) const noexcept
{
  return DataString(value + toAppend.value);
}

DataString&
DataString::operator=(const DataString& toCopyAssign) noexcept
{
  value = Data()
}

DataString&
DataString::operator=(const DataString&& toMoveAssign) noexcept
{
  value = std::move(toMoveAssign);
}

DataString::operator herald::datatype::Data() const noexcept
{
  return value;
}

DataString::iterator
DataString::begin() const noexcept
{
  return DataStringIterator(*this, 0);
}

DataString::iterator
DataString::end() const noexcept
{
  return DataStringIterator(*this, size());
}

DataStringIterator::DataStringIterator(const DataString& stringOver, size_t at) noexcept
  : pos(at),
    over(stringOver)
{
}

DataStringIterator::~DataStringIterator() noexcept = default;

char
DataStringIterator::operator*() noexcept
{
  if (pos < over.size()) {
    return over.at[pos];
  }
  return '\0';
}

bool
DataStringIterator::operator==(const DataStringIterator& other) const noexcept
{
  return (pos == other.pos) && (over == other.over);
}
bool
DataStringIterator::operator!=(const DataStringIterator& other) const noexcept
{
  return (pos != other.pos) || (over != other.over);
}
void
DataStringIterator::operator++() noexcept
{
  ++pos;
  if (pos > over.size()) {
    pos = over.size();
  }
}
void
DataStringIterator::operator--() noexcept
{
  if (pos > 0) {
    --pos;
  }
}

DataStringStream::DataStringStream() noexcept
  : value()
{
}

DataStringStream::DataStringStream(DataStringStream&& toMove) noexcept
  : value(std::move(toMove.value))
{
}

DataStringStream::~DataStringStream() noexcept = default;

DataStringStream&
DataStringStream::operator<<(DataString consume) noexcept
{
  value.append((Data)consume);
}

DataStringStream&
DataStringStream::operator<<(std::size_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(std::uint8_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(std::uint16_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(std::uint32_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(std::uint64_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(std::int8_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(std::int16_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(std::int32_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(std::int64_t consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(long consume) noexcept
{
  // TODO convert to string representation
}

DataStringStream&
DataStringStream::operator<<(char consume) noexcept
{
  value.append(consume);
}

DataStringStream&
DataStringStream::operator<<(EOSStruct consume) noexcept
{
  // No Op for our representation
}


DataString
DataStringStream::str() noexcept
{
  return DataString(value);
}

/// \brief Implicit conversion to Data for Data.append() to work
/// \note DOES NOT conver to HEX. Equivalent of raw char array access
DataStringStream::operator herald::datatype::Data() const noexcept
{
  return Data(value);
}



#endif


}  // namespace data
}  // namespace herald

#ifdef CONFIG_HERALD_NO_STD_STRING
#ifndef CONFIG_HERALD_NO_STD_STREAMS
namespace std {

std::ostream& operator<<(std::ostream& lhs,
                         const herald::data::String& rhs) noexcept
{
  for (auto c : rhs) {
    lhs << c;
  }
  return lhs;
}

}
#endif
#endif
