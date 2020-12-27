CROSS_COMPILE=/home/cn/Project/BUAA/embedding/exp3/03_tools/gcc-linaro-5.4.1-2017.05-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
INC=.

CFLAGS=-I${INC} -march=armv7 -std=c++11
LDFLAGS= -lstdc++ 

CXX=${CROSS_COMPILE}g++
CC=${CROSS_COMPILE}gcc

SRC=./util.cpp testcase/interoperability.c

all: gpio_in_ext gpio_out gpio_in
test: test_interoperability

test_interoperability: util.o testcase/interoperability.o
	g++  ${CFLAGS} ${LDFLAGS} $^ -o $@ 

%.o: %.cpp
	${CXX} ${CFLAGS} -c -o $@ $^

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $^

%.o: testcase/%.c
	${CXX} ${CFLAGS} -c -o $@ $^

gpio_in_ext: gpio_in_ext.c GPIO_CTL.h
	${CXX} -o gpio_in_ext gpio_in_ext.c -lpthread

gpio_out: gpio_out.c GPIO_CTL.h
	${CXX} -o gpio_out gpio_out.c

gpio_in: gpio_in.o util.o GPIO_CTL.h 
	${CXX} gpio_in.o util.o -o gpio_in 

.PHONY: clean test

clean:
	rm -f *.o testcase/*.o
	rm -f gpio_in gpio_out gpio_in_ext
	rm -rf ./test_interoperability
