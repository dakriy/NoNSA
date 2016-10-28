#compiler flags
CC = g++
CFLAGS = -g -Wall 
CXXFLAGS = -std=c++14 -I/usr/include/boost/

SRCS=main.cpp logo.cpp keybase.cpp external/SimpleJSON/src/JSON.cpp external/SimpleJSON/src/JSONValue.cpp SocketWrapper.cpp Message.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
LIBS=-lboost_thread -lboost_system -lpthread

all: nonsa

nonsa: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(CXXFLAGS) -o nonsa $(OBJS)

clean:
	$(RM) nonsa *.o *~

run:
	./nonsa
