#pragma once
#include <yaml-cpp/yaml.h>

namespace CCDB {
class Config {
public:
  Config() {
    YAML::Node config = YAML::LoadFile("../application.json");

    if (config["rpc"]) {
      std::cout << config["rpc"]["port"].as<int>() << "\n";
    }

    // const std::string username = config["username"].as<std::string>();
    // const std::string password = config["password"].as<std::string>();
  }
};
} // namespace CCDB