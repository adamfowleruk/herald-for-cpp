// Copyright 2022 Herald Project Contributors
// SPDX-License-Identifier: Apache-2.0
//

/**
 * @file Implements the wrapping of a MESH modem client with a link to RabbitMQ.
 */

#ifdef HERALD_RABBITMQ

#include "herald/mesh/modem_rabbitmq.h"
#include "herald/mesh/modem_client.h"


#include <amqpcpp.h>

#include <iostream>

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <errno.h>    // Error integer and strerror() function
#include <fcntl.h>    // Contains file controls like O_RDWR
#include <termios.h>  // Contains POSIX terminal control definitions
#include <unistd.h>   // write(), read(), close()

namespace herald {
namespace mesh {

namespace internal {

// TODO make this less nasty, and allow for multiple RabbitMqAdapter instances

MeshModemRabbitMQAdapter* globalModemAdapter = NULL;

int global_read_fn(uint8_t* buffer, size_t sz) {
  if (NULL != globalModemAdapter) {
    return globalModemAdapter->read(buffer, sz);
  }
  return 0;
}

RmqHandler::RmqHandler(uv_loop_t* loop) : AMQP::LibUvHandler(loop) {}

RmqHandler::~RmqHandler() = default;

void
RmqHandler::onError(AMQP::TcpConnection* connection, const char* message)
{
  std::cout << " (async from rmq) error: " << message << std::endl;
}

void 
RmqHandler::onConnected(AMQP::TcpConnection* connection)
{
  std::cout << " (async from rmq) connected" << std::endl;
}

} // end internal namespace

MeshModemRabbitMQAdapter::MeshModemRabbitMQAdapter()
 : rmqUrl(),
   rmqExchangeName(),
   rmqCommandQueueName(),
   rmqRoutingKey(),
   rmqHandler(),
   rmqConnection(),
   rmqChannel(),
   mdmFilePath(),
   tty(),
   mdmPortNumber(0),
   modem(),
   ready(false)
{
  // Do nothing - all done by configure() and connect()
}

MeshModemRabbitMQAdapter::~MeshModemRabbitMQAdapter() {
  disconnect(true);
  // do nothing else (everything uses smart pointers)
}

bool MeshModemRabbitMQAdapter::configure(uv_loop_t* uvloop,
                                         std::string amqpUrl,
                                         std::string exchangeName,
                                         std::string commandQueueName,
                                         std::string routingKey,
                                         std::string modemTtyFile) {
  // TODO input validation

  // LibUV settings
  loop = uvloop;

  // RabbitMQ settings
  rmqUrl = amqpUrl;
  rmqExchangeName = exchangeName;
  rmqCommandQueueName = commandQueueName;
  rmqRoutingKey = routingKey;

  // MESH Modem settings
  mdmFilePath = modemTtyFile;

  return true; // TEMPORARY UNTIL VALIDATION IMPLEMENTED
}

bool
MeshModemRabbitMQAdapter::connect()
{
  ready = false;

  // TODO check if already assigned
  // TODO ensure configure has occurred

  // 1. Connect to RabbitMQ and declare objects

  // declare handler
  rmqHandler.emplace(loop);

  // create a AMQP connection object

  std::cout << "  - Creating connection" << std::endl;
  AMQP::Address address(rmqUrl);
  rmqConnection.emplace(
    &*rmqHandler,
    address
  );

  // and create a channel
  std::cout << "  - Creating channel" << std::endl;
  rmqChannel.emplace(&*rmqConnection);

  std::cout << "  - Creating exchange" << std::endl;
  // use the channel object to call the AMQP method you like
  rmqChannel->declareExchange(rmqExchangeName,
                             AMQP::fanout);  // TODO verify exchange type

  std::cout << "  - Creating queue" << std::endl;
  rmqChannel
    ->declareQueue(rmqCommandQueueName,
                   AMQP::durable + AMQP::autodelete + AMQP::exclusive)
    .onSuccess([](const std::string& name, uint32_t messagecount,
                  uint32_t consumercount) {
      // report the name of the temporary queue
      std::cout << " (async from rmq) declared queue " << name << std::endl;
    });
  std::cout << "  - Binding queue" << std::endl;
  rmqChannel->bindQueue(rmqExchangeName, rmqCommandQueueName, rmqRoutingKey);

  // TODO set up heartbeat for RabbitMQ

  // 2. Attempt to connect to the local modem device

  // Open the serial port
  std::cout << "  - Opening modem port" << std::endl;
  int sp = open(mdmFilePath.c_str(), O_RDWR);  // read and write
  if (sp < 0) {
    printf("Error %i opening modem port file: %s\n", errno, strerror(errno));
    return false;
  }
  mdmPortNumber = sp;

  // Read in existing settings, and handle any error
  std::cout << "  - Reconfiguring modem port" << std::endl;
  if (0 != tcgetattr(mdmPortNumber, &tty)) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    return false;
  }

  tty.c_cflag &= ~PARENB;  // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB;  // Clear stop field, only one stop bit used in
                           // communication (most common)
  tty.c_cflag &= ~CSIZE;   // Clear all bits that set the data size
  tty.c_cflag |= CS8;      // 8 bits per byte (most common)
  tty.c_cflag &=
      ~CRTSCTS;  // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |=
      CREAD | CLOCAL;  // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;    // Disable echo
  tty.c_lflag &= ~ECHOE;   // Disable erasure
  tty.c_lflag &= ~ECHONL;  // Disable new-line echo
  tty.c_lflag &= ~ISIG;    // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                   ICRNL);  // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST;  // Prevent special interpretation of output bytes
                          // (e.g. newline chars)
  tty.c_oflag &=
      ~ONLCR;  // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT
  // PRESENT ON LINUX) tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars
  // (0x004) in output (NOT PRESENT ON LINUX)

  tty.c_cc[VTIME] = 10;  // Wait for up to 1s (10 deciseconds), returning as
                         // soon as any data is received.
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate to be 115200
  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);

  // Save tty settings, also checking for error
  if (tcsetattr(mdmPortNumber, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    return false;
  }

  // Now finally pass this open file to the MeshModem class
  std::cout << "  - Creating MeshModem wrapper" << std::endl;
  modem.emplace(mdmPortNumber, 
    bt_mesh_modem_client_cb {
      .read = &herald::mesh::internal::global_read_fn
    }
  );

  ready = true;

  // Now both sides are ready, start processing
  std::cout << "  - Resuming modem" << std::endl;
  modem->resume();

  // callback function that is called when the consume operation starts
  auto startCb = [](const std::string& consumertag) {
    std::cout << " (async from rmq) RabbitMQ consume operation started"
              << std::endl;
  };

  // callback function that is called when the consume operation failed
  auto errorCb = [](const char* message) {
    std::cout << " (async from rmq) RabbitMQ consume operation failed"
              << std::endl;
  };

  // callback operation when a message was received
  auto messageCb = [this](const AMQP::Message& message, uint64_t deliveryTag,
                          bool redelivered) {
    std::cout << " (async from rmq) RabbitMQ message received" << std::endl;

    // TODO pass this along to the MeshModem as a write()

    // acknowledge the message
    rmqChannel->ack(deliveryTag);
  };

  // start consuming from the queue, and install the callbacks
  std::cout << "  - Subscribing to modem commands" << std::endl;
  rmqChannel->consume(rmqCommandQueueName)
    .onReceived(messageCb)
    .onSuccess(startCb)
    .onError(errorCb);

  std::cout << "  - Connected!" << std::endl;
  return true;
}

int
MeshModemRabbitMQAdapter::read(uint8_t* buffer, size_t sz)
{
  std::cout << " (async from rmq) Reading " << sz << " bytes of data"
            << std::endl;
  // TODO read something from serial port
  // TODO determine protobuf message type
  // TODO send to appropriate queue
  return sz; // TODO replace this with actual read size
}

bool
MeshModemRabbitMQAdapter::disconnect(bool immediate)
{
  ready = false; // stop processing immediately

  // Disconnect from the local file path
  modem->pause();
  modem.reset();
  close(mdmPortNumber);
  mdmPortNumber = 0;

  if (isConnected()) {
    // TODO disconnect from RabbitMQ
  }

  return true;
}

bool
MeshModemRabbitMQAdapter::isConnected()
{
  return ready;
}

} // end namespace
} // end namespace

#endif