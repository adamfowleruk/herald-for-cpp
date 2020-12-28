//  Copyright 2020 VMware, Inc.
//  SPDX-License-Identifier: Apache-2.0
//

#include "datatype/data.h"

#include <algorithm>
#include <iterator>
#include <vector>

namespace herald {
namespace datatype {

// DATA PIMPL DEFINITIONS

class Data::Impl {
public:
  Impl();
  ~Impl() = default;

  std::vector<std::byte> data;
};

// DATA PIMPL DECLARATIONS
Data::Impl::Impl() : data() { }





// DATA CLASS DECLARATIONS
Data::Data() : mImpl(std::make_unique<Impl>()) {
  ;
}

Data::Data(Data&& other)
  : mImpl(std::move(other.mImpl))
{
  ;
}

Data::Data(const std::byte* value, std::size_t length) : mImpl(std::make_unique<Impl>()) {
  mImpl->data.reserve(length);
  for (std::size_t i = 0;i < length; i++) {
    mImpl->data.push_back(value[i]);
  }
}

Data::Data(std::vector<std::byte> value) : mImpl(std::make_unique<Impl>()) {
  mImpl->data = std::move(value);
}

Data::Data(const Data& from) : mImpl(std::make_unique<Impl>()) {
  mImpl->data = from.mImpl->data; // copy ctor
}

Data::Data(std::byte repeating, std::size_t count) : mImpl(std::make_unique<Impl>()) {
  mImpl->data.reserve(count);
  for (std::size_t i = 0;i < count; i++) {
    mImpl->data.push_back(repeating);
  }
}

Data&
Data::operator=(const Data& other)
{
  mImpl->data = other.mImpl->data;
  return *this;
}

Data::~Data() {}




std::string
Data::description() const {
  return ""; // TODO make this a real description
}

Data
Data::subdata(std::size_t offset) const {
  Data copy;
  if (offset > mImpl->data.size()) {
    return std::move(copy);
  }
  std::copy(mImpl->data.begin() + offset, mImpl->data.end(), std::back_inserter(copy.mImpl->data));
  return std::move(copy);
}

Data
Data::subdata(std::size_t offset, std::size_t length) const {
  Data copy;
  if (offset + length > mImpl->data.size()) {
    return std::move(copy);
  }
  std::copy(mImpl->data.begin() + offset, mImpl->data.begin() + offset + length, std::back_inserter(copy.mImpl->data));
  return std::move(copy);
}

std::byte
Data::at(std::size_t index) const {
  if (index > mImpl->data.size() - 1) {
    return std::byte(0);
  }
  return mImpl->data[index];
}

void
Data::append(const Data& data) {
  mImpl->data.reserve(mImpl->data.size() + data.size());
  std::copy(data.mImpl->data.begin(), data.mImpl->data.end(), std::back_inserter(mImpl->data));
}
void
Data::append(uint8_t data)
{
  std::size_t curSize = mImpl->data.size();
  mImpl->data.reserve(curSize + 1); // C++ ensures types are AT LEAST x bits
  mImpl->data[curSize++] = std::byte(data);
}
void
Data::append(uint16_t data)
{
  std::size_t curSize = mImpl->data.size();
  mImpl->data.reserve(curSize + 2); // C++ ensures types are AT LEAST x bits
  mImpl->data[curSize++] = std::byte(data >> 8);
  mImpl->data[curSize++] = std::byte(data & 0xff);
}
void
Data::append(uint32_t data)
{
  std::size_t curSize = mImpl->data.size();
  mImpl->data.reserve(curSize + 4); // C++ ensures types are AT LEAST x bits
  mImpl->data[curSize++] = std::byte(data >> 24);
  mImpl->data[curSize++] = std::byte(data >> 16);
  mImpl->data[curSize++] = std::byte(data >> 8);
  mImpl->data[curSize++] = std::byte(data & 0xff);
}
void
Data::append(uint64_t data)
{
  std::size_t curSize = mImpl->data.size();
  mImpl->data.reserve(curSize + 8); // C++ ensures types are AT LEAST x bits
  mImpl->data[curSize++] = std::byte(data >> 56);
  mImpl->data[curSize++] = std::byte(data >> 48);
  mImpl->data[curSize++] = std::byte(data >> 40);
  mImpl->data[curSize++] = std::byte(data >> 32);
  mImpl->data[curSize++] = std::byte(data >> 24);
  mImpl->data[curSize++] = std::byte(data >> 16);
  mImpl->data[curSize++] = std::byte(data >> 8);
  mImpl->data[curSize++] = std::byte(data & 0xff);
}
void
Data::append(const std::string& data)
{
  std::size_t curSize = mImpl->data.size();
  mImpl->data.reserve(curSize + data.size());
  for (std::size_t i = 0;i < data.size();i++) {
    mImpl->data[i] = std::byte(data[i]);
  }
}

bool
Data::operator==(const Data& other) const noexcept {
  //if (hashCode() != other.hashCode()) {
  //  return false;
  //}
  // else compare each value

  // alternatively, cheat...
  return hashCode() == other.hashCode(); // Somewhat naughty
}


std::size_t
Data::hashCode() const {
  return std::hash<std::vector<std::byte>>{}(mImpl->data);
}

std::size_t
Data::size() const {
  return mImpl->data.size();
}

} // end namespace
} // end namespace