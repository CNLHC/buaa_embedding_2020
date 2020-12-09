CROSS_COMPILE=''
INC=.

CFLAGS=-I${INC}
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

gpio_in: gpio_in.o GPIO_CTL.h 
	${CXX} -o gpio_in gpio_in.o util.o

.PHONY: clean test

clean:
	rm -f *.o testcase/*.o
	rm -rf ./test_interoperability
