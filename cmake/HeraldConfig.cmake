# SPDX-License-Identifier: Apache-2.0

# This file provides Herald config version package functionality.
#

# Those are Herald variables used.
set(Herald_VERSION ${Herald_VERSION})
set(Herald_MAJOR_MINOR ${Herald_VERSION_MAJOR}.${Herald_VERSION_MINOR})
set(Herald_VERSION ${Herald_VERSION_MAJOR}.${Herald_VERSION_MINOR}.${Herald_VERSION_PATCH})
set(Herald_MAJOR_MINOR_MICRO ${Herald_VERSION})

get_filename_component(HERALD_INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)
set(HERALD_INSTALL_DIR ${HERALD_INSTALL_DIR})
set(HERALD_BASE ${HERALD_INSTALL_DIR}/herald)

# Those are CMake package parameters.
set(Herald_FOUND True)
set(Herald_DIR   ${HERALD_INSTALL_DIR})
