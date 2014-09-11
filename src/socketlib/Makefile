# makefile for the Queue class example

TARGET=$(shell uname -m)

CC=gcc
CXX=g++
AR=ar
RM=rm -f
CPPFLAGS=-g -Wall -Wwrite-strings -fpic

LDLIBS=-L build/$(TARGET)/lib

SRCS_LIBS=tcpconnector.cpp  tcpstream.cpp tcpacceptor.cpp
OBJS_LIBS=$(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_LIBS))))

MAKELIB=libsocket.so


all: display $(MAKELIB)

dist-clean: clean
	$(RM) *~ .dependtool

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX)  $(CPPFLAGS) -MM $^>>./.depend;


$(MAKELIB): $(OBJS_LIBS)
	$(CXX) -shared -o $@ $(OBJS_LIBS)


%.o: %.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
	

clean:
	$(RM) $(OBJS_LIBS) $(MAKELIB)

display:
	@echo "SRCS_LIBS='$(SRCS_LIBS)'"
	@echo "OBJS_LIBS='$(OBJS_LIBS)'"
