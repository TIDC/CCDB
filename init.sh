#!/usr/bin/env bash

set -e

cpu=$(cat /proc/cpuinfo | grep processor | wc -l)

make_install() {
  echo $1
  echo "编译 "$1
  make -j ${cpu}
  echo "安装 "$1
  make install
}

install_pack() {
  name=$1
  dir=$2
  file=$3
  url=$4
  cmake_params=$5

  echo "-----------------------------------------------------------------"
  echo $name
  if [ -d $dir ]; then
    rm -fr $dir
  fi

  echo "下载 "$name
  wget https://ghproxy.com/$url
  echo "解压 "$name
  tar -zxvf $file

  echo "删除 $name 压缩包"
  rm -fr $file
  cd $dir
  mkdir build
  cd build
  echo "编译 "$name
  cmake -DCMAKE_BUILD_TYPE=Release $cmake_params ..

  make_install $name

  cd ../../
}

if [ ! -d "./lib" ]; then
  echo "创建 lib 文件夹"
  mkdir ./lib
fi

echo "进入 lib 文件夹"
cd lib


if [ ! -d "/usr/local/include/gflags" ]; then
  install_pack \
    gflags \
    gflags-2.2.2 \
    v2.2.2.tar.gz \
    https://github.com/gflags/gflags/archive/refs/tags/v2.2.2.tar.gz
else
  echo "gflags 已存在跳过安装"
fi

if [ ! -d "/usr/include/rocksdb" ]; then
  install_pack \
    rocksdb \
    rocksdb-7.5.3 \
    v7.5.3.tar.gz \
    https://github.com/facebook/rocksdb/archive/refs/tags/v7.5.3.tar.gz \
    -DROCKSDB_BUILD_SHARED=OFF
else
  echo "rocksdb 已存在跳过安装"
fi

if [ ! -f "/usr/local/include/zmq.h" ]; then
  echo "-----------------------------------------------------------------"
  if [ -d "libzmq" ]; then
    rm -fr libzmq
  fi
  echo "下载 libzmq"
  git clone git@git.zhlh6.cn:zeromq/libzmq
  cd libzmq
  echo "下载 libtool"
  apt install libtool

  echo "自动构建 autogen"
  ./autogen.sh

  echo "配置 configure"
  ./configure

  make_install libzmq

  cd ../
else
  echo "libzmq 已存在跳过安装"
fi


if [ ! -f "/usr/local/include/zmq.hpp" ]; then
  install_pack \
    cppzmq \
    cppzmq-4.8.1 \
    v4.8.1.tar.gz \
    https://github.com/zeromq/cppzmq/archive/refs/tags/v4.8.1.tar.gz \
    -DCPPZMQ_BUILD_TESTS=OFF
else
  echo "cppzmq 已存在跳过安装"
fi

if [ ! -d "/usr/local/include/yaml-cpp" ]; then
  install_pack \
    yaml-cpp \
    yaml-cpp-yaml-cpp-0.7.0 \
    yaml-cpp-0.7.0.tar.gz \
    https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.tar.gz
else
  echo "yaml-cpp 已存在跳过安装"
fi

echo "初始化结束！依赖安装完成！"