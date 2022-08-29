#include "./db/index.hpp"
#include "./rpc/index.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "argv: " << argv << std::endl;
  int i;
  std::cout << "参数个数: " << argc << std::endl;
  for (i = 0; i < argc; i++) {
    std::cout << "参数序号: " << i << std::endl;
    std::cout << "参数值: " << argv[i] << std::endl;
  }

  TestZmq();
}