//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#include "herald/data/devnull_logging_sink.h"

#include "herald/data/sensor_logger.h"
#include "herald/data/string_utils.h"

#include <iostream>

namespace herald::data {

DevNullLoggingSink::DevNullLoggingSink() = default;
DevNullLoggingSink::~DevNullLoggingSink() = default;

void
DevNullLoggingSink::log(const String& subsystem, const String& category, SensorLoggerLevel level, const String& message)
{
  ; // Literally do nothing... like cat-ing to /dev/null
}

}