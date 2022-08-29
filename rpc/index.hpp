#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <zmq.hpp>

// class Rpc {
// public:
//   Rpc() {

//     // initialize the zmq context with a single IO thread
//     zmq::context_t context{1};
//     // construct a REP (reply) socket and bind to interface
//     socket_ = zmq::socket_t{context, zmq::socket_type::rep};
//   }

//   void listen() { socket_.bind("tcp://*:5555"); }

// private:
//   zmq::socket_t socket_;
// }

int TestZmq() {
  zmq::context_t context{1};
  // construct a REP (reply) socket and bind to interface
  zmq::socket_t socket{context, zmq::socket_type::rep};
  // prepare some static data for responses
  const std::string data{"World"};

  for (;;) {
    zmq::message_t request;

    // receive a request from client
    socket.recv(request, zmq::recv_flags::none);
    std::cout << "Received " << request.to_string() << std::endl;
    socket.send(zmq::buffer(data), zmq::send_flags::none);
    // const std::string data2{"World2"};
    // socket.send(zmq::buffer(data2), zmq::send_flags::none);
  }

  return 0;
}