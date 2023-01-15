//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/data/stdout_logging_sink.h"

#include "herald/data/sensor_logger.h"
#include "herald/data/string_utils.h"

#ifdef CONFIG_HERALD_NO_STD_STREAMS
#include <cstdio>
#else
#include <iostream>
#endif

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

#ifdef CONFIG_HERALD_NO_STD_STREAMS
  printf("%s,%s,%s,%s\n",
    subsystem.c_str(),category.c_str(),lvl.c_str(),message.c_str());
#else
  std::cout << subsystem.c_str() << "," << category.c_str() << ","
            << lvl.c_str() << "," << message.c_str() << std::endl;
#endif
}

}