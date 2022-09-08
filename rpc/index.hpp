#pragma once

#include <chrono>
#include <deque>
#include <iostream>
#include <string>

#include <zmq.hpp>

// #include "general_message.pb.h"

#include <array>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <thread>

#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <zmq.hpp>

namespace CCDB {

class Rpc {
public:
  Rpc() {
    std::cout << "初始化 RPC: " << std::endl;
    // initialize the zmq context with a single IO thread
    ctx_ = std::make_shared<zmq::context_t>(1);
    // construct a REP (reply) socket and bind to interface
    // socket_ = std::make_shared<zmq::socket_t>(ctx_, zmq::socket_type::rep);
    std::cout << "初始化 RPC 结束" << std::endl;
  }

  /// 链接 rpc 服务
  void Bind() {
    assert(ctx_ != nullptr);

    // NOTE: cppzmq 封装的 zmq::socket_t 在析构时会自动断开连接，所以重复调用
    // Connect 函数，如果有，会先断开先前的连接
    socket_ = std::make_unique<zmq::socket_t>(*ctx_, zmq::socket_type::rep);
    assert(socket_ != nullptr && "Out Of Memory!!!");
    // socket_->set(zmq::sockopt::routing_id, uuid_);
    socket_->bind("tcp://*:5555");
    std::cout << "bind: tcp://localhost:5555" << std::endl;
  }

  void Listen() {
    for (;;) {
      int count = 0;
      std::deque<zmq::message_t> message;
      do {
        std::cout << "读取信息: " << std::endl;
        zmq::message_t msg;
        auto result = socket_->recv(msg, zmq::recv_flags::none);
        std::cout << "Received slice: " << msg.to_string() << std::endl;
        if (result.has_value()) {
          message.emplace_back(std::move(msg));
        }
        const std::string data{"World back"};
        // socket_->send(zmq::buffer(data), zmq::send_flags::none);
      } while (socket_->get(zmq::sockopt::rcvmore));

      std::cout << "Received:" << std::endl;
      for (zmq::message_t &msg : message) {
        std::cout << msg.to_string() << std::endl;
      }
      const std::string data{"World back"};
      socket_->send(zmq::buffer(data), zmq::send_flags::none);
    }
  }

private:
  std::shared_ptr<zmq::context_t> ctx_{};
  std::unique_ptr<zmq::socket_t> socket_{};
};

constexpr auto SERVER_CONNECT_ADDR = "tcp://localhost:5570";
constexpr auto SERVER_BIND_ADDR = "tcp://*:5570";
constexpr auto SERVER_CORE_BIND_ADDR = "tcp://*:9999";
constexpr auto SERVER_CORE_CONNECT_ADDR = "tcp://localhost:9999";

std::array<char, 10> UUID() {
  timeval t;
  gettimeofday(&t, nullptr);
  srandom(t.tv_sec + t.tv_usec + syscall(SYS_gettid));
  std::array<char, 10> id;
  sprintf(id.data(), "%04lX-%04lX", random() % 0x10000, random() % 0x10000);
  return id;
}

/// 接收 zmq 的全部分块消息
std::vector<zmq::message_t> ReceiveAll(zmq::socket_t &socket) {
  std::vector<zmq::message_t> message;
  do {
    zmq::message_t msg;
    auto result = socket.recv(msg, zmq::recv_flags::none);
    if (result.has_value()) {
      message.emplace_back(std::move(msg));
    }
  } while (socket.get(zmq::sockopt::rcvmore));

  return message;
}

void PrintMessage(const std::vector<zmq::message_t> &msgs) {
  for (const auto &m : msgs) {
    std::cout << std::string_view{static_cast<const char *>(m.data()), m.size()}
              << " | ";
  }
  std::cout << std::endl;
}

void Client() {
  auto id = UUID();
  std::string_view id_sv{id.data()};
  auto ctx = zmq::context_t{1};

  std::cout << "init createWorker" << std::endl;
  auto createWorker = zmq::socket_t{ctx, zmq::socket_type::req};
  createWorker.connect(SERVER_CORE_CONNECT_ADDR);
  std::cout << "申请创建 worker" << std::endl;
  createWorker.send(zmq::message_t{id_sv.data(), id_sv.size()});
  zmq::message_t reply{};
  createWorker.recv(reply, zmq::recv_flags::none);
  std::cout << "Received " << reply.to_string();
  createWorker.close();

  auto client = zmq::socket_t{ctx, zmq::socket_type::dealer};
  client.set(zmq::sockopt::routing_id, id.data());
  client.connect(SERVER_CONNECT_ADDR);
  std::cout << std::string_view{id.data()} << std::endl;

  std::vector<zmq::pollitem_t> items;
  items.push_back({client.handle(), 0, ZMQ_POLLIN, 0});

  size_t request_number = 0;
  while (true) {
    for (size_t tick = 0; tick < 100; tick++) {

      auto poll_result = zmq::poll(items, std::chrono::milliseconds(10));
      assert(poll_result >= 0);
      if (poll_result == 0) {
        continue;
      }
      for (auto &item : items) {
        if (item.events & ZMQ_POLLIN) {
          auto msgs = ReceiveAll(client);
          std::cout << id_sv << " 收到服务端的回复: ";
          PrintMessage(msgs);
        }
      }
    }

    request_number++;
    std::string str{"request #"};
    str += std::to_string(request_number);
    std::cout << id_sv << " 客户端发送：" << str << std::endl;
    client.send(zmq::message_t{str.data(), str.size()}, zmq::send_flags::none);
  }
}

void ServerWorker(std::string ipc_addr, std::string_view router_id) {
  std::cout << "worker 启动 router_id: " << router_id << std::endl;
  auto id = UUID();
  std::string_view id_sv{id.data()};
  std::string r(router_id.data());
  auto ctx = zmq::context_t{1};
  auto socket = zmq::socket_t{ctx, zmq::socket_type::dealer};
  socket.set(zmq::sockopt::routing_id, router_id.data());
  socket.connect(ipc_addr);
  // socket.connect("tcp://localhost:5555");
    std::cout << "r = " << r << std::endl;

  while (true) {
    std::cout << "worker 等待消息" << std::endl;
    auto message = ReceiveAll(socket);
    assert(message.size() >= 2);
    std::string_view client_uuid{message[0].data<char>(), message[0].size()};
    std::string_view client_message{message[1].data<char>(), message[1].size()};
    assert(!client_message.empty());
    std::cout << id_sv << " 收到来自路由" << r << "客户端的消息：";
    PrintMessage(message);

    size_t replise = random() % 5;
    for (size_t reply = 0; reply < replise; reply++) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds((random() % 1000) + 1));
      zmq::const_buffer buffer{client_uuid.data(), client_uuid.size()};
      socket.send(buffer, zmq::send_flags::sndmore);
      zmq::const_buffer content{client_message.data(), client_message.size()};
      socket.send(content, zmq::send_flags::sndmore);
      socket.send(buffer);
    }
  }
}

void Server() {
  auto id = UUID();
  std::string_view id_sv{id.data()};
  auto ctx = zmq::context_t{1};

  auto frontend = zmq::socket_t{ctx, zmq::socket_type::router};
  frontend.bind(SERVER_BIND_ADDR);

  auto backend = zmq::socket_t{ctx, zmq::socket_type::dealer};
  // 让系统自动分配端口号
  std::string backend_addr{"tcp://*:0"};
  backend.bind(backend_addr);
  // 获取实际 bind 的地址
  backend_addr = backend.get(zmq::sockopt::last_endpoint);

  auto core = zmq::socket_t{ctx, zmq::socket_type::rep};
  core.bind(SERVER_CORE_BIND_ADDR);
  std::vector<std::thread> worker;

  worker.emplace_back([&] {
    while (true) {
      std::cout << "core 等待消息" << std::endl;
      zmq::message_t message;
      core.recv(message, zmq::recv_flags::none);
      std::cout << "core 收到消息 " << message.to_string() << std::endl;

      std::string_view client_uuid{message.data<char>(), message.size()};
      std::cout << client_uuid << " 创建 worker";
      worker.emplace_back(ServerWorker, backend_addr, client_uuid);
      std::string str{"request #"};
      core.send(zmq::message_t{str.data(), str.size()}, zmq::send_flags::none);
    }
  });

  std::cout << "proxy 启动" << std::endl;
  zmq::proxy(frontend, backend);

  for (auto &thr : worker) {
    if (thr.joinable()) {
      thr.join();
    }
  }
}
} // namespace CCDB