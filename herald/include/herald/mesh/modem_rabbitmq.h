// Copyright 2022 Herald Project Contributors
// SPDX-License-Identifier: Apache-2.0
//

/**
 * @file Wraps a MESH modem client with a link to RabbitMQ.
 */

#ifndef HERALD_MESH_MODEM_RABBITMQ_H
#define HERALD_MESH_MODEM_RABBITMQ_H

#ifdef HERALD_RABBITMQ

#include "modem_client.h"

// AMQP-CPP is Apache-2.0 licensed
// Note: Its TCP layer only supports Linux (So using libuv instead)
#include <amqpcpp.h>
#include <amqpcpp/libuv.h>
#include <uv.h>

namespace herald {
namespace mesh {

namespace internal {

class RmqHandler : public AMQP::LibUvHandler {
private:
  /**
   *  Method that is called when a connection error occurs
   *  @param  connection
   *  @param  message
   */
  virtual void onError(AMQP::TcpConnection* connection,
                       const char* message) override;

  /**
   *  Method that is called when the TCP connection ends up in a connected state
   *  @param  connection  The TCP connection
   */
  virtual void onConnected(AMQP::TcpConnection* connection) override;

public:
  /**
   *  Constructor
   *  @param  uv_loop
   */
  RmqHandler(uv_loop_t* loop);

  /**
   *  Destructor
   */
  virtual ~RmqHandler();
};

} // end internal namespace

class MeshModemRabbitMQAdapter {
public:
  MeshModemRabbitMQAdapter();
  ~MeshModemRabbitMQAdapter();

  /**
   * @brief Validate and save this configuration
   * 
   * @param amqpUrl 
   * @param exchangeName 
   * @param commandQueueName 
   * @param routingKey 
   * @param modemTtyFile 
   * @return true If configuration valid and accepted
   * @return false If not (configuration will not be reset)
   */
  bool configure(uv_loop_t* uvloop,
                 std::string amqpUrl, std::string exchangeName,
                 std::string commandQueueName, std::string routingKey,
                 std::string modemTtyFile);

  /**
   * @brief Attempt to connect to RabbitMQ, and try to stay connected.
   * 
   * @return true If connection, exchange declare, and queue bind calls accepted
   * @return false If calls failed
   */
  bool connect();

  /**
   * @brief Disconnect from RabbitMQ. 
   * @note The destructor calls this method for you
   * @note This method checks isConnected() for you
   * 
   * @param immediate Whether to force an immediate disconnection (defaults to false)
   * @return true If disconnected (whether already disconnected, or just disconnected)
   * @return false If disconnection call failed
   */
  bool disconnect(bool immediate = false);

  /**
   * @brief Returns whether we are currently connected to RabbitMQ
   * @note This may return false after connect() called if the connection
   * has temporarily dropped out but not yet been reestablished.
   * 
   * @return true If actually connected to RabbitMQ
   * @return false If not currently connected (may be retrying behind the scenes)
   */
  bool isConnected();

  // TODO figure out how to make this private!
  int read(uint8_t* buffer, size_t sz);

 private:
  // LibUV info
  uv_loop_t* loop;

  // RabbitMQ details
  std::string rmqUrl;
  std::string rmqExchangeName;
  std::string rmqCommandQueueName;
  std::string rmqRoutingKey;
  std::optional<herald::mesh::internal::RmqHandler> rmqHandler;
  std::optional<AMQP::TcpConnection> rmqConnection;
  std::optional<AMQP::TcpChannel> rmqChannel;

  // TTY (CDC ACM over USBD) Herald Bluetooth MESH Modem link
  std::string mdmFilePath;
  struct termios tty;
  int mdmPortNumber;
  std::optional<herald::mesh::MeshModem> modem;

  // overall state
  bool ready;
};


} // end namespace
} // end namespace

#endif

#endif // end HERALD_MESH_MODEM_RABBITMQ_H