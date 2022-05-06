# the compiler: gcc for C program, define as g++ for C++
CC = clang++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS= -g -Wall
LINKERFLAG=-I/usr/local/include -I/usr/local/include/httpserver -I/usr/include/p11-kit-1 -L/usr/local/lib -lcurl -lhttpserver -lmicrohttpd -pthread

# The build target 
TARGET=loadlists

all: $(TARGET)
$(TARGET): $(TARGET).cpp
	$(CC) -std=c++17 $(CFLAGS) $(LINKERFLAG) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)