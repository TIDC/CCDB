#include "./db/index.hpp"
#include "./rpc/index.hpp"
#include <thread>
// #include "./config/index.hpp"

int main(int argc, char *argv[]) {
  std::thread server{CCDB::Server};
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::thread client1{CCDB::Client};
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::thread client2{CCDB::Client};
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::thread client3{CCDB::Client};

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}

// int main(int argc, char *argv[]) {
//   // TestDB();
//   CCDB::Rpc rpc;
//   rpc.Bind();
//   rpc.Listen();
// }

// #include <iostream>
// using namespace std;

// void Reverse(int *A, int from, int to) {
//   int mid = (from + to + 1) / 2;
//   for (int i = from; i < mid; i++) {
//     int t = A[i];
//     A[i] = A[to - i + from];
//     A[to - i + from] = t;
//   }
// }

// void Converse(int *arr, int n, int k) {
//   int q = k % n;
//   Reverse(arr, 0, q - 1);
//   Reverse(arr, q, n - 1);
//   Reverse(arr, 0, n - 1);
// }

// int main(int argc, char *argv[]) {
//   int n, m;
//   cout << "请输入元素个数:";
//   cin >> n;
//   cout << "请输入元素:";
//   int arr[n];
//   for (int i = 0; i < n; i++) {
//     cin >> arr[i];
//   }
//   cout << "请输入数组左移位置数:";
//   cin >> m;
//   Converse(arr, n, m);
//   cout << "数组循环左移 " << m << "个位置后，数组元素为：" << endl;
//   for (int i = 0; i < n; i++) {
//     cout << arr[i] << " ";
//   }
//   cout << endl;

//   cout << "学好：啥啥,姓名：啥啥" << endl;
// }

// int main(int argc, char *argv[]) {
//   // std::cout << "argv: " << argv << std::endl;
//   // int i;
//   // std::cout << "参数个数: " << argc << std::endl;
//   // for (i = 0; i < argc; i++) {
//   //   std::cout << "参数序号: " << i << std::endl;
//   //   std::cout << "参数值: " << argv[i] << std::endl;
//   // }

//   // TestZmq();

//   // CCDB::Config();
// }