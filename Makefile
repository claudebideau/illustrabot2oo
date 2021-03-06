# makefile for the Queue class example

TARGET=$(shell uname -m)

CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g -Wall -Wwrite-strings -Isrc/include -Ibuild/$(TARGET)/include 
# -std=c++0x -DILLUSOO_THREAD=1
ifeq (1,${TEST})
CPPFLAGS += -D__TEST
endif
ILLUSOO_LDFLAGS = -l xmlrpc++ -l xmlrpc_server++ -l xmlrpc_server_abyss++ -lsocket  -lrt -lpthread -Wl
DEAMON_LDFLAGS = -lsocket  -lrt -lpthread 

LDLIBS=-L build/$(TARGET)/lib -L src/socketlib

SRCS_DEFAULT=src/rttrace.cpp  src/trace.c
SRCS_INIREAD=src/inireader.cpp
# SRCS_SOCKET=src/socket/Socket.cpp src/server.cpp  src/clientTh.cpp src/socket/ServerSocket.cpp src/socketThCl.cpp
SRCS_EASYDRV=src/gpio.cpp src/easydrv.cpp  src/ms_element.cpp src/angle.cpp  src/arm.cpp src/hand.cpp src/orientation.cpp
SRCS_INITEST=initst.cpp $(SRCS_INIREAD)
SRCS_XML=src/rpc/element.cpp src/rpc/rttraceRpc.cpp src/rpc/traceRpc.cpp src/rpc/osRpc.cpp src/rpc/armRpc.cpp src/rpc/orientationRpc.cpp
SRCS_SRV=  src/illustrabot2srv.cpp  src/thread/RobotClientTh.cpp $(SRCS_DEFAULT) $(SRCS_XML) $(SRCS_EASYDRV) $(SRCS_INIREAD)
SRCS_DEBUG= src/debug_thread.cpp $(SRCS_DEFAULT) $(SRCS_XML) $(SRCS_EASYDRV) $(SRCS_INIREAD)
SRCS_DEAMON= src/serverDeamon.cpp src/thread/RobotSrvTh.cpp src/thread/UeSrvTh.cpp  src/thread/UeSrvStreamTh.cpp   src/thread/DebugSrvTh.cpp $(SRCS_DEFAULT) $(SRCS_INIREAD) 
SRCS= $(SRCS_SRV) $(SRCS_DEAMON)

OBJS_DEFAULT=$(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_DEFAULT))))
OBJS_INIREAD=$(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_INIREAD))))
OBJS_SOCKET= $(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_SOCKET))))
OBJS_EASYDRV= $(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_EASYDRV))))
OBJS_INITEST= $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_INITEST)))
OBJS_XML= $(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_XML))))
OBJS_SRV= $(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_SRV))))
OBJS_DEAMON= $(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_DEAMON))))
OBJS_DEBUG= $(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_DEBUG))))
OBJS=$(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS))))

TOOLS=illustrabot2srv serverDeamon

OBJDIR=obj/rpc obj/socket obj/thread


all: display $(TOOLS)

dist-clean: clean
	$(RM) *~ .dependtool

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX)  $(CPPFLAGS) -MM $^>>./.depend;


illustrabot2srv: display $(OBJS_SRV)  | $(OBJDIR)
	$(CXX) $(OBJS_SRV) -o $@ $(LDLIBS) $(ILLUSOO_LDFLAGS)    

serverDeamon: display $(OBJS_DEAMON)  | $(OBJDIR)
	$(CXX) $(OBJS_DEAMON) -o $@ $(LDLIBS) $(DEAMON_LDFLAGS)  
    

obj/%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/rpc/%.o: src/rpc/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/socket/%.o: src/socket/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/thread/%.o: src/thread/%.c
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
    
obj/%.o: src/%.c
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
    
%.o: %.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
	

clean:
	$(RM) $(OBJS) $(TOOLS)

display:
	@echo "SRCS_EASYDRV='$(SRCS_EASYDRV)'"
	@echo "OBJS_EASYDRV='$(OBJS_EASYDRV)'"
	@echo "SRCS_INITEST='$(SRCS_INITEST)'"
	@echo "OBJS_INITEST='$(OBJS_INITEST)'"
	@echo "SRCS_XML    ='$(SRCS_XML)'"
	@echo "OBJS_XML    ='$(OBJS_XML)'"
	@echo "SRCS_SOCKET ='$(SRCS_SOCKET)'"
	@echo "OBJS_SOCKET ='$(OBJS_SOCKET)'"
	mkdir -p $(OBJDIR)

