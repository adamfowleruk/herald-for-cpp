// Copyright 2022 Herald Project Contributors
// SPDX-License-Identifier: Apache-2.0
//

/**
 * @file Wraps a MESH modem client with a link to RabbitMQ.
 */

#ifndef HERALD_MESH_MODEM_RABBITMQ_H
#define HERALD_MESH_MODEM_RABBITMQ_H

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

// class MeshAdapterTcpHandler : public AMQP::TcpHandler {
//   MeshAdapterTcpHandler();
//   ~MeshAdapterTcpHandler();
  
//   /**
//    *  Method that is called by the AMQP library when a new connection
//    *  is associated with the handler. This is the first call to your handler
//    *  @param  connection      The connection that is attached to the handler
//    */
//   virtual void onAttached(AMQP::TcpConnection* connection) override;

//   /**
//    *  Method that is called by the AMQP library when the TCP connection
//    *  has been established. After this method has been called, the library
//    *  still has take care of setting up the optional TLS layer and of
//    *  setting up the AMQP connection on top of the TCP layer., This method
//    *  is always paired with a later call to onLost().
//    *  @param  connection      The connection that can now be used
//    */
//   virtual void onConnected(AMQP::TcpConnection* connection) override;

//   /**
//    *  Method that is called when the secure TLS connection has been established.
//    *  This is only called for amqps:// connections. It allows you to inspect
//    *  whether the connection is secure enough for your liking (you can
//    *  for example check the server certificate). The AMQP protocol still has
//    *  to be started.
//    *  @param  connection      The connection that has been secured
//    *  @param  ssl             SSL structure from openssl library
//    *  @return bool            True if connection can be used
//    */
//   virtual bool onSecured(AMQP::TcpConnection* connection,
//                          const SSL* ssl) override;

//   /**
//    *  Method that is called when the server tries to negotiate a heartbeat
//    *  interval, and that is overridden to get rid of the default implementation
//    *  (which vetoes the suggested heartbeat interval), and accept the interval
//    *  instead.
//    *  @param  connection      The connection on which the error occurred
//    *  @param  interval        The suggested interval in seconds
//    */
//   virtual uint16_t onNegotiate(AMQP::TcpConnection *connection, uint16_t interval) override;

//   /**
//    *  Method that is called by the AMQP library when the login attempt
//    *  succeeded. After this the connection is ready to use.
//    *  @param  connection      The connection that can now be used
//    */
//   virtual void onReady(AMQP::TcpConnection* connection) override;

//   /**
//    *  Method that is called by the AMQP library when a fatal error occurs
//    *  on the connection, for example because data received from RabbitMQ
//    *  could not be recognized, or the underlying connection is lost. This
//    *  call is normally followed by a call to onLost() (if the error occurred
//    *  after the TCP connection was established) and onDetached().
//    *  @param  connection      The connection on which the error occurred
//    *  @param  message         A human readable error message
//    */
//   virtual void onError(AMQP::TcpConnection* connection,
//                        const char* message) override;

//   /**
//    *  Method that is called when the AMQP protocol is ended. This is the
//    *  counter-part of a call to connection.close() to graceful shutdown
//    *  the connection. Note that the TCP connection is at this time still
//    *  active, and you will also receive calls to onLost() and onDetached()
//    *  @param  connection      The connection over which the AMQP protocol ended
//    */
//   virtual void onClosed(AMQP::TcpConnection* connection) override;

//   /**
//    *  Method that is called when the TCP connection was closed or lost.
//    *  This method is always called if there was also a call to onConnected()
//    *  @param  connection      The connection that was closed and that is now
//    * unusable
//    */
//   virtual void onLost(AMQP::TcpConnection* connection) override;

//   /**
//    *  Final method that is called. This signals that no further calls to your
//    *  handler will be made about the connection.
//    *  @param  connection      The connection that can be destructed
//    */
//   virtual void onDetached(AMQP::TcpConnection* connection) override;

//   /**
//    *  Method that is called by the AMQP-CPP library when it wants to interact
//    *  with the main event loop. The AMQP-CPP library is completely non-blocking,
//    *  and only make "write()" or "read()" system calls when it knows in advance
//    *  that these calls will not block. To register a filedescriptor in the
//    *  event loop, it calls this "monitor()" method with a filedescriptor and
//    *  flags telling whether the filedescriptor should be checked for readability
//    *  or writability.
//    *
//    *  @param  connection      The connection that wants to interact with the
//    * event loop
//    *  @param  fd              The filedescriptor that should be checked
//    *  @param  flags           Bitwise or of AMQP::readable and/or AMQP::writable
//    */
//   virtual void monitor(AMQP::TcpConnection* connection, int fd,
//                        int flags) override;
// };

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

#endif // end HERALD_MESH_MODEM_RABBITMQ_H