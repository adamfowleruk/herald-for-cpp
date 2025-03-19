# SPDX-License-Identifier: Apache-2.0

# The purpose of this CMake file is to register the Herald package in:
#
#   Linux/macOS: ~/.cmake/packages/Herald
#       Windows: HKEY_CURRENT_USER
#
# By registering the Herald package, the Herald build system can locate the
# required version of Herald through `find_package(Herald)` even when
# HERALD_INSTALL_DIR is not explicitly set.
#
# Register the Herald package by running `cmake -P herald_export.cmake`
# in this directory.

if(WIN32)
  set(MD5_PATH ${CMAKE_CURRENT_LIST_DIR})
else()
  set(MD5_PATH "/tmp")
endif()

set(MD5_INFILE "current_path.txt")

# We write CMAKE_CURRENT_LIST_DIR into MD5_INFILE, as the content of that file
# will be used for MD5 calculation.  This means we effectively get the MD5 of
# CMAKE_CURRENT_LIST_DIR which must be used for CMake user package registry.
file(WRITE ${MD5_PATH}/${MD5_INFILE} ${CMAKE_CURRENT_LIST_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E md5sum ${MD5_PATH}/${MD5_INFILE}
                OUTPUT_VARIABLE MD5_SUM
)
string(SUBSTRING ${MD5_SUM} 0 32 MD5_SUM)
if(WIN32)
  execute_process(COMMAND ${CMAKE_COMMAND}
                  -E write_regv
                  "HKEY_CURRENT_USER\\Software\\Kitware\\CMake\\Packages\\Herald\;${MD5_SUM}"
                  "${CMAKE_CURRENT_LIST_DIR}"
  )
else()
  file(WRITE $ENV{HOME}/.cmake/packages/Herald/${MD5_SUM} ${CMAKE_CURRENT_LIST_DIR})
endif()

message("Herald (${CMAKE_CURRENT_LIST_DIR})")
message("has been added to the user package registry in:")
if(WIN32)
  message("HKEY_CURRENT_USER\\Software\\Kitware\\CMake\\Packages\\Herald\n")
else()
  message("~/.cmake/packages/Herald\n")
endif()

file(REMOVE ${MD5_PATH}/${MD5_INFILE})
