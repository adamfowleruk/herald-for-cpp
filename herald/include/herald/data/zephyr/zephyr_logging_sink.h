//  Copyright 2020-2021 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_ZEPHYR_LOGGING_SINK_H
#define HERALD_ZEPHYR_LOGGING_SINK_H

#include "../sensor_logger.h"
#include "../string_utils.h"

// NOTE: Link Herald to the Zephyr logging system
// Set HERALD_LOG_LEVEL=4 for debug in CMake using add_definitions(-DHERALD_LOG_LEVEL=4 )
//   Defaults to 0 (OFF) - see herald/data/zephyr/zephyr_logging_sink.h
// #include <logging/log.h>

namespace herald {
namespace data {

class ZephyrLoggingSink {
public:
  ZephyrLoggingSink() noexcept = default;
  ~ZephyrLoggingSink() noexcept = default;

  void log(const herald::data::String& subsystem,
           const herald::data::String& category, SensorLoggerLevel level,
           const herald::data::String& message) const noexcept;
};

} // end namespace
} // end namespace

#endif