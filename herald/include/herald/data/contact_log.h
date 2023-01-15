//  Copyright 2020-2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

#ifndef HERALD_CONTACT_LOG_H
#define HERALD_CONTACT_LOG_H

#include "../sensor_delegate.h"
#include "payload_data_formatter.h"
#include "sensor_logger.h"
#include "../context.h"
#include "string_utils.h"

namespace herald::data {

/**
 * Logs all contact info to STDERR to allow it to be extracted as a stream
 */
template <typename ContextT, typename PayloadDataFormatterT>
class ErrorStreamContactLogger {
public:
  ErrorStreamContactLogger(ContextT& context, PayloadDataFormatterT& formatter)
    : ctx(context), 
      fmt(formatter)
      HLOGGERINIT(ctx,"Sensor","contacts.log")
  {
    ;
  }
  ~ErrorStreamContactLogger() = default;

  // Sensor delegate overrides
  void sensor(SensorType sensor, const TargetIdentifier& didDetect) {
    HTDBG("didDetect");
  }
  // TODO Log all the other activity
  // void sensor(SensorType sensor, const PayloadData& didRead, const TargetIdentifier& fromTarget) {}
  // void sensor(SensorType sensor, const ImmediateSendData& didReceive, const TargetIdentifier& fromTarget) {}
  // void sensor(SensorType sensor, const std::vector<PayloadData>& didShare, const TargetIdentifier& fromTarget){}
  // void sensor(SensorType sensor, const Proximity& didMeasure, const TargetIdentifier& fromTarget) {}
  // template <typename LocationT>
  // void sensor(SensorType sensor, const Location<LocationT>& didVisit) {}
  // void sensor(SensorType sensor, const Proximity& didMeasure, const TargetIdentifier& fromTarget, const PayloadData& withPayload) {}
  // void sensor(SensorType sensor, const SensorState& didUpdateState) {}

private:
  herald::data::String csv(herald::data::String toEscape) const noexcept {
    // C++23 only: if (toEscape.contains(",") || toEscape.contains("\"") ||
    // toEscape.contains("'") || toEscape.contains("’")) { Pre C++23:-
    if (herald::data::String::npos != toEscape.find(',') ||
        herald::data::String::npos != toEscape.find('"') ||
        herald::data::String::npos != toEscape.find('\'') ||
        herald::data::String::npos != toEscape.find('`')) {
      return '"' + toEscape + '"';
    }
    return toEscape;
  }

  herald::data::String timestamp() const noexcept {
    return Date().iso8601DateTime();
  }

  ContextT& ctx;
  PayloadDataFormatterT& fmt;

  HLOGGER(ContextT);
};

}

#endif