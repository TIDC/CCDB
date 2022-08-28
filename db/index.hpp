#pragma once
#include "rocksdb/db.h"
#include <iostream>
// #include <assert>

void TestDB() {
  rocksdb::DB *db;
  rocksdb::Options options;
  options.create_if_missing = true;
  rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);

  std::cout << "status: " <<  status.ok() << std::endl;
  // assert(status.ok());
}