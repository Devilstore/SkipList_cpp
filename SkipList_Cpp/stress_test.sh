
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
g++ stress_test/stress_test.cpp -w -o ./bin/stress_test --std=c++11 -pthread

rm -f *.o

# 运行
./bin/stress_test
