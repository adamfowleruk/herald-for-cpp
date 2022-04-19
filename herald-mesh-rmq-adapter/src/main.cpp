//  Copyright 2022 Herald Project Contributors
//  SPDX-License-Identifier: Apache-2.0
//

// TODO fail to build currently if not done on Linux 
//      (RMQ Library platform support for TCP is limited.)

/**
 * @file The application binary for a Herald Bluetooth MESH
 * and RabbitMQ Adapter connection.
 */
#include "herald/mesh/modem_rabbitmq.h"

#include <amqpcpp.h>
#include <uv.h>

#include <csignal>
#include <cstdlib>
#include <iomanip>
#include <iostream>

// Declare our singleton adapter (one per app instance)
herald::mesh::MeshModemRabbitMQAdapter adapter;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;

  // cleanup and close up stuff here
  if (adapter.isConnected()) {
    std::cout << "Adapter disconnecting..." << std::endl;
    bool disconnected = adapter.disconnect();
    if (disconnected) {
      std::cout << "Adapter disconnected successfully. Quitting." << std::endl;
    } else {
      std::cerr << "Adapter did not disconnect successfully. Quitting anyway."
                << std::endl;
    }
  } else {
    std::cout << "Adapter already disconnected. Quitting." << std::endl;
  }

  // TODO any uvloop cleanup

  // terminate program
  exit(signum);
}

int main(int argc, char *argv[]) {
  // The app name, plus:-
  //  - 5 arguments for RabbitMQ
  //  - 1 optional argument for --no-openssl
  if (6 > argc) {
    std::cerr << "Usage: herald-mesh-adapter <amqpurl> <exchangeName> <commandQueueName> <routingKey> <modemTtyFilePath>" << std::endl;
    return EXIT_FAILURE;
  }
  // if (7 == argc) {
  //   // If we've not been told to not initialise openssl dynamically, then do it!
  //   if (0 != strcmp("--no-openssl",argv[6])) {
  //     // dynamically open the openssl library
  //     // TODO allow this to be dynamically specified via --openssldir and --prefix (for '/lib/')
  //     void* handle = dlopen("/usr/local/ssl/lib/openssl-3.so", RTLD_LAZY);

  //     // tell AMQP-CPP library where the handle to openssl can be found
  //     AMQP::openssl(handle);

  //     OPENSSL_init_ssl(0, NULL);
  //   }
  // }

  auto* loop = uv_default_loop();

  // Configure the RabbitMQ Adapter
  bool configured = adapter.configure(loop, argv[1], argv[2], argv[3], 
                                      argv[4], argv[5]);
  if (!configured) {
    // fail with message
    std::cerr << "Failed to configure RabbitMQ adapter with configuration:-"
              << std::endl;
    std::cerr << "  amqpurl:          " << argv[1] << std::endl;
    std::cerr << "  exchangeName:     " << argv[2] << std::endl;
    std::cerr << "  commandQueueName: " << argv[3] << std::endl;
    std::cerr << "  routingKey:       " << argv[4] << std::endl;
    std::cerr << "  modemTtyFilePath: " << argv[5] << std::endl;
    return EXIT_FAILURE;
  }

  // TODO Dynamic RMQ library and OpenSSL library detection.
  //      Quit if either is not detected.

  // Now connect to RMQ so we can send all status information
  bool connected = adapter.connect();
  if (!connected) {
    std::cerr << "Could not connect to RabbitMQ so failing" << std::endl;
    return EXIT_FAILURE;
  }

  // TODO try to connect to tty specified too

  // TODO pass initial MESH network connection info


  // Enter an infinite loop until we get a SIGTERM
  uv_run(loop, UV_RUN_DEFAULT);

  return EXIT_SUCCESS;
}