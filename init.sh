error_check() {
  if [ $? -ne 0 ]; then
    echo "!!!!!!!!!!!!!!!!!! 出错 !!!!!!!!!!!!!!!!!!"
    echo "!!!!!!!!!!!!!!!!!! 出错 !!!!!!!!!!!!!!!!!!"
    echo "!!!!!!!!!!!!!!!!!! 出错 !!!!!!!!!!!!!!!!!!"
    echo "!!!!!!!!!!!!!!!!!! 出错 !!!!!!!!!!!!!!!!!!"
    echo "!!!!!!!!!!!!!!!!!! 出错 !!!!!!!!!!!!!!!!!!"
    echo "!!!!!!!!!!!!!!!!!! 出错 !!!!!!!!!!!!!!!!!!"
    echo "!!!!!!!!!!!!!!!!!! 出错 !!!!!!!!!!!!!!!!!!"

    exit $?
  fi
}

get_cpu_count() {
  cpu=2
  if [ "$(uname)" == 'Darwin' ]; then
    cpu=$(sysctl -n machdep.cpu.thread_count)
  elif [ "$(expr substr $(uname -s) 1 5)"=="Linux" ]; then
    cpu=$(cat /proc/cpuinfo | grep processor | wc -l)
  fi
  return $cpu
}

get_cpu_count
cpu=$?

if [ ! -d "./lib" ]; then
  echo "创建 lib 文件夹"
  mkdir ./lib
fi

error_check
echo "进入 lib 文件夹"
cd lib
error_check

if [ ! -d "/usr/local/include/gflags" ]; then
  echo "-----------------------------------------------------------------"
  echo "前置 rocksdb 依赖 gflags"
  if [ -d "./gflags-2.2.2" ]; then
    rm -fr ./gflags-2.2.2
  fi

  echo "下载 gflags"
  wget https://ghproxy.com/https://github.com/gflags/gflags/archive/refs/tags/v2.2.2.tar.gz
  echo "解压 gflags"
  tar -zxvf v2.2.2.tar.gz
  error_check
  echo "删除 gflags 压缩包"
  rm -fr ./v2.2.2.tar.gz
  cd gflags-2.2.2
  mkdir build
  cd build
  echo "编译 gflags"
  cmake -DCMAKE_BUILD_TYPE=Release ..
  error_check
  make -j ${cpu}
  error_check
  echo "安装 gflags"
  make install
  error_check
  cd ../../
else
  echo "gflags 已存在跳过安装"
fi

if [ ! -d "/usr/include/rocksdb" ]; then
  echo "-----------------------------------------------------------------"
  if [ -d "rocksdb-7.5.3" ]; then
    rm -fr rocksdb-7.5.3
  fi
  ls
  echo "下载 rocksdb"
  wget https://ghproxy.com/https://github.com/facebook/rocksdb/archive/refs/tags/v7.5.3.tar.gz
  error_check
  echo "解压 rocksdb"
  tar -zxvf v7.5.3.tar.gz
  error_check
  echo "删除 rocksdb 压缩包"
  rm -fr ./v7.5.3.tar.gz
  cd rocksdb-7.5.3
  mkdir build
  cd build
  echo "编译 rocksdb"
  cmake -DROCKSDB_BUILD_SHARED=OFF -DCMAKE_BUILD_TYPE=Release ..
  error_check
  make -j ${cpu}
  error_check
  echo "安装 rocksdb"
  make install
  error_check
  cd ../../
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
  # wget https://ghproxy.com/https://github.com/zeromq/libzmq/archive/refs/tags/v4.3.4.tar.gz
  # error_check
  # tar -zxvf v4.3.4.tar.gz

  error_check
  # echo "删除 libzmq 压缩包"
  # rm -fr ./v4.3.4.tar.gz
  cd libzmq
  echo "下载 libtool"
  apt install libtool
  error_check
  echo "自动构建 autogen"
  ./autogen.sh
  error_check
  echo "配置 configure"
  ./configure
  error_check
  make -j ${cpu}
  error_check
  echo "安装 libzmq"
  make install
  error_check
  echo "安装 libzmq"
  ldconfig
  error_check
  cd ../
  # cd libzmq-4.3.4
  # mkdir build
  # cd build
  # echo "编译 libzmq"
  # cmake -DCMAKE_BUILD_TYPE=Release ..
  # error_check
  # make -j ${cpu}
  # error_check
  # echo "安装 libzmq"
  # make install
  # error_check
  # cd ../../
else
  echo "libzmq 已存在跳过安装"
fi

if [ ! -f "/usr/local/include/zmq.hpp" ]; then
  echo "-----------------------------------------------------------------"
  if [ -d "cppzmq-4.8.1" ]; then
    rm -fr cppzmq-4.8.1
  fi
  echo "下载 cppzmq"
  wget https://ghproxy.com/https://github.com/zeromq/cppzmq/archive/refs/tags/v4.8.1.tar.gz
  error_check
  tar -zxvf v4.8.1.tar.gz

  error_check
  echo "删除 cppzmq 压缩包"
  rm -fr ./v4.8.1.tar.gz
  cd cppzmq-4.8.1
  mkdir build
  cd build
  echo "cmake cppzmq"
  cmake -DCMAKE_BUILD_TYPE=Release -DCPPZMQ_BUILD_TESTS=OFF ..
  error_check
  echo "编译 cppzmq"
  make -j ${cpu}
  error_check
  echo "安装 cppzmq"
  make install
  error_check
  cd ../../
else
  echo "cppzmq 已存在跳过安装"
fi

echo "初始化结束！依赖安装完成！"
