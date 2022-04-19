# Copyright 2020-2022 Herald Project Contributors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.12)
project(herald-mesh-adapter VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(NO_BUILD_TYPE_WARNING ON)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
find_package(OpenSSL 3.0.2 REQUIRED)
find_package(amqpcpp REQUIRED)
find_package(libuv REQUIRED)

set(HERALD_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../herald)
include(../herald/herald.cmake) # find sources and headers

include_directories(../herald/include)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DHERALD_LOG_LEVEL=4)
else()
  add_definitions(-DHERALD_LOG_LEVEL=0)
endif()

add_executable(herald-mesh-adapter
  ${HERALD_SOURCES_MESH_CLIENT}

	# main application file
	src/main.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_include_directories(herald-mesh-adapter SYSTEM PRIVATE
  ${OPENSSL_INCLUDE_DIR}
  ${AMQPCPP_INCLUDE_DIR}
  ${LIBUV_INCLUDE_DIRS}
)

target_link_libraries(herald-mesh-adapter PRIVATE 
  amqpcpp 
  ${LIBUV_LIBRARIES}
  Threads::Threads
  dl
  OpenSSL::SSL
)

target_compile_features(herald-mesh-adapter PRIVATE cxx_std_17)
