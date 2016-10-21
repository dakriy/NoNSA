#compiler flags
CC = g++
CFLAGS = -g -Wall 
CXXFLAGS = -std=c++14

SRCS=main.cpp logo.cpp keybase.cpp external/SimpleJSON/src/JSON.cpp external/SimpleJSON/src/JSONValue.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: nonsa

nonsa: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(CXXFLAGS) -o nonsa $(OBJS)

clean:
	$(RM) nonsa *.o *~

run:
	./nonsa
