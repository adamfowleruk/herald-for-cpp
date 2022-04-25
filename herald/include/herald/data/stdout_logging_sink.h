//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_STDOUT_LOGGING_SINK
#define HERALD_STDOUT_LOGGING_SINK

#include "herald/data/sensor_logger.h"
#include "herald/data/string_utils.h"

namespace herald{
namespace data {

struct StdOutLoggingSink {
  StdOutLoggingSink();
  ~StdOutLoggingSink();

  void log(const String& subsystem, const String& category, SensorLoggerLevel level, const String& message);
};

}
}

#endif