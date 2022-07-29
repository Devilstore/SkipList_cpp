
# 测试脚本
#!/bin/bash


CRTDIR=$(pwd)

if [ ! -d "${CRTDIR}/bin" ]; then
    mkdir -p ${CRTDIR}/bin
fi

if [ ! -d "${CRTDIR}/store" ]; then
    mkdir -p ${CRTDIR}/store
fi


# 编译
g++ main.cpp -o ./bin/skiplist --std=c++11 -pthread

rm -f *.o
# 运行
./bin/skiplist
