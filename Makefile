# makefile for the Queue class example

TARGET=$(shell uname -m)

CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g  -Wwrite-strings -Isrc/include -Ibuild/$(TARGET)/include
ifeq (1,${TEST})
CPPFLAGS += -D__TEST
endif
LDFLAGS = -l xmlrpc++ -l xmlrpc_server++ -l xmlrpc_server_abyss++  -lrt  -Wl 

LDLIBS=-L build/$(TARGET)/lib

SRCS_INIREAD=src/inireader.cpp
SRCS_EASYDRV=src/gpio.cpp src/easydrv.cpp src/rttrace.cpp  src/trace.c  src/ms_element.cpp
SRCS_INITEST=initst.cpp $(SRCS_INIREAD)
SRCS_XML=src/rpc/element.cpp src/rpc/rttraceRpc.cpp src/rpc/traceRpc.cpp  src/debug_thread.cpp $(SRCS_EASYDRV) $(SRCS_INIREAD)
SRCS= $(SRCS_INITEST) $(SRCS_XML) 

OBJS_INIREAD=$(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_INIREAD))))
OBJS_EASYDRV= $(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_EASYDRV))))
OBJS_INITEST= $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_INITEST)))
OBJS_XML= $(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS_XML))))
OBJS=$(subst .c,.o, $(subst .cpp,.o, $(subst src/,obj/,$(SRCS))))

TOOLS=debug_thread

OBJDIR=obj/rpc


all: display $(TOOLS)

dist-clean: clean
	$(RM) *~ .dependtool

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX)  $(CPPFLAGS) -MM $^>>./.depend;

debug_thread: $(OBJS_XML)  | $(OBJDIR)
	$(CXX) $(OBJS_XML) -o $@ $(LDLIBS) $(LDFLAGS)

ini: $(OBJS_INITEST)
	$(CXX) $(OBJS_INITEST) -o ini

obj/%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/rpc/%.o: src/rpc/%.cpp
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
	mkdir -p $(OBJDIR)

