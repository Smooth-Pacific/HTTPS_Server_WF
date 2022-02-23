.PHONY = all

CC=clang++

SRCS := $(wildcard *.cpp)
NAME := $(SRCS:%.cpp=%)

LINKERFLAG=-I/usr/local/include -I/usr/local/include/httpserver -I/usr/include/p11-kit-1 -L/usr/local/lib -lhttpserver -lmicrohttpd -lgnutls -pthread -g

all: ${SRCS}
	${CC} -std=c++17 ${LINKERFLAG} -o prog $< -lstdc++fs
