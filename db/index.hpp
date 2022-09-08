#pragma once
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include <iostream>
#include <string_view>
#include <thread>
// #include <assert>
using ROCKSDB_NAMESPACE::ColumnFamilyDescriptor;
using ROCKSDB_NAMESPACE::ColumnFamilyHandle;
using ROCKSDB_NAMESPACE::ColumnFamilyOptions;
using ROCKSDB_NAMESPACE::DB;
using ROCKSDB_NAMESPACE::DBOptions;
using ROCKSDB_NAMESPACE::Options;
using ROCKSDB_NAMESPACE::ReadOptions;
using ROCKSDB_NAMESPACE::Slice;
using ROCKSDB_NAMESPACE::Status;
using ROCKSDB_NAMESPACE::WriteBatch;
using ROCKSDB_NAMESPACE::WriteOptions;
void TestDB() {

  // open DB
  Options options;
  options.create_if_missing = true;
  DB *db;
  Status s;

  // s = DB::Open(options, "/tmp/testdb", &db);
  // assert(s.ok());

  // // create column family
  // ColumnFamilyHandle *cf;
  // s = db->CreateColumnFamily(ColumnFamilyOptions(), "new_cf", &cf);
  // assert(s.ok());

  // // close DB
  // s = db->DestroyColumnFamilyHandle(cf);
  // assert(s.ok());
  // delete db;

  // open DB with two column families
  std::vector<ColumnFamilyDescriptor> column_families;
  // have to open default column family
  column_families.push_back(ColumnFamilyDescriptor(
      ROCKSDB_NAMESPACE::kDefaultColumnFamilyName, ColumnFamilyOptions()));
  // open the new one, too
  column_families.push_back(
      ColumnFamilyDescriptor("new_cf", ColumnFamilyOptions()));
  std::vector<ColumnFamilyHandle *> handles;
  s = DB::Open(DBOptions(), "/tmp/testdb", column_families, &handles, &db);
  std::cout << "db status: " << s.ToString() << std::endl;
  assert(s.ok());
  s = db->SetOptions(handles[0], {{"ttl", "3"}});
  std::cout << "SetOptions status: " << s.ok() << std::endl;

  db->Put(rocksdb::WriteOptions(), handles[0], "ttl1", "ttl1ttl1");
  db->Put(rocksdb::WriteOptions(), handles[1], "nottl1", "nottl1nottl1");

  // Do some 'work'
  std::this_thread::sleep_for(std::chrono::seconds(4));

  std::string str;
  s = db->Get(rocksdb::ReadOptions(), handles[0], "ttl1", &str);
  std::cout << "ttl1 status: " << s.ok() << std::endl;
  std::cout << "ttl1: " << str << std::endl;
  s = db->Get(rocksdb::ReadOptions(), handles[1], "nottl1", &str);
  std::cout << "nottl1 status: " << s.ok() << std::endl;
  std::cout << "nottl1: " << str << std::endl;
  assert(s.ok());
}