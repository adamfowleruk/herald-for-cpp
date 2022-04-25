//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_HEX_STRING_H
#define HERALD_HEX_STRING_H

#include "data.h"
#include "herald/data/string_utils.h"

namespace herald {
/// \brief Contains all low-level Herald datatype implementations
namespace datatype {

/// \brief Strongly, rather than stringly, typed representation of Base64 String data.
/// Prevents incorrect initialisation or manipulation
class HexString {
public:
  HexString(); // empty string initialiser
  /// \brief Move Constructor
  HexString(HexString&& other);
  /// \brief Deleted copy constructor to prevent temporary memory use
  HexString(const HexString& other) = delete;

  // DO NOT use these, use HexString::encode instead
  // /// \brief Create a Hex representation of a raw data instance
  // HexString(const Data& copyFrom);
  // /// \brief Create a Hex representation of a raw data instance, by moving 
  // /// the Data representation 
  // /// \note Useful for dynamically created Data instances, such as the
  // /// result from data.reversed()
  // HexString(Data&& moveFrom);
  
  /// \brief Custom destructor
  ~HexString();

  /// \brief Populates a HexString from a herald::data::String of hex characters
  static bool from(const herald::data::String& original, HexString& toInitialise) noexcept; // initialise from string

  /// \brief Populates a HexString from a C string (const char*) of hex characters
  static bool from(const char* original, HexString& toInitialise) noexcept;

  /// \brief Creates a HexString from an arbitrary set of bytes
  /// \sa Data
  static HexString encode(const Data& from) noexcept; // initialise from Data

  /// \brief Decodes this HexString's content into a Data instance
  /// \sa Data
  Data decode() const noexcept;

  /// \brief Creates a representation (copy) of this HexString's data decoded
  /// value (not chars of the string)
  operator Data() const noexcept;
  
  /// \brief Returns the Base16/Hex encoding of this class as a herald::data::String
  herald::data::String encoded() const noexcept; // Return base16/hex string representation (copy of, not reference to)
private:
  herald::data::String value; // Base16/Hex encoded, and guarded
};

} // end namespace
} // end namespace

#endif