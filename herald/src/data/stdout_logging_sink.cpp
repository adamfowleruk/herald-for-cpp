//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/data/stdout_logging_sink.h"

#include "herald/data/sensor_logger.h"
#include "herald/data/string_utils.h"

#include <iostream>

namespace herald::data {

StdOutLoggingSink::StdOutLoggingSink() = default;
StdOutLoggingSink::~StdOutLoggingSink() = default;

void
StdOutLoggingSink::log(const String& subsystem, const String& category, SensorLoggerLevel level, const String& message)
{
  String lvl = "info";
  switch (level) {
    case SensorLoggerLevel::debug:
      lvl = "debug";
      break;
    case SensorLoggerLevel::fault:
      lvl = "fault";
      break;
    default:
      break;
  }
  std::cout << subsystem << "," << category << ","
            << lvl << "," << message << std::endl;
}

}