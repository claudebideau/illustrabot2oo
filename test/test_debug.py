#!/usr/bin/env python

import sys
import time
import binascii
import socket
import struct,array

import asyncore

class dbgClientCl(asyncore.dispatcher):

    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__header_packer = struct.Struct('B B B B i i i')
        self.__stat_packer = struct.Struct('i i i i i i')
        self.__state = -1
        self.__txcnt = 0
        self.__rxcnt = 0
        self.connect( (host, port) )
        self.__queue=[]
        self.__statistic=[(0,0,0,0,0,0) for i in range(3)]

    def handle_connect(self):
        pass

    def handle_close(self):
        self.close()

    def handle_read(self):
        data = self.recv(8192)
        while len(data) >0:
            if len(data) >= 16:
                unpacked_header=self.__header_packer.unpack(data[0:16])
                data = data[16:]
                print 'rx_header=', unpacked_header
                if unpacked_header[0] == 1:
                    self.__rxcnt = unpacked_header[6]
                    if self.__state==0 and unpacked_header[1]==0x60:
                        values = (1, 33, self.__state, 0, 0, self.__txcnt,self.__rxcnt )
                        packed_data = self.__header_packer.pack(*values)
                        self.__txcnt += 1
                        sent = self.send(packed_data)
                        self.__state=1
                    if self.__state==1 and unpacked_header[1]==0x61:
                        print 'connected'
                        self.__state=2
                    if self.__state==2:
                        # __stat
                        if unpacked_header[1] in [0x68, 0x69]:
                            if unpacked_header[4] != 1024*4: raise Exception('stat size issue')
                            if len(data) >= 4096:
                                trace=array.array('I',data[0:4096])
                                print 'rx stat= ',trace
                                data = data[4096:]
                        # __trace
                        if unpacked_header[1] in [0x65, 0x66, 0x67]:
                            if unpacked_header[4] != 24: raise Exception('stat size issue')
                            if len(data) >= 24:
                                unpacked_stat=self.__stat_packer.unpack(data[0:24])
                                self.__storeStat( unpacked_header[2]&0x3F-0x25, unpacked_stat)
                                print 'rx stat= ',unpacked_stat
                                data = data[24:]

    def __storeStat(self,code,value):
        self.__statistic[code] = value
        return

    def getStat(self,code):
        return self.__statistic[code]
                
    def statReq(self,socketid=0):
        values = (1, 0x25+socketid, 0, 0, 0, self.__txcnt,self.__rxcnt )
        print 'request stat !!!', values
        self.__queue.append(values)

    def traceReq(self,socketid=0):
        values = (1, 0x28+socketid, 0, 0, 0, self.__txcnt,self.__rxcnt )
        print 'request trace !!!', values
        self.__queue.append(values)
        

    def writable(self):
#        print 'writable?', len(self.__queue)
        return len(self.__queue)> 0

    def handle_write(self):
        if self.__state==-1:
            values = (1, 32, 0, 0, 0, self.__txcnt,self.__rxcnt )
            packed_data = self.__header_packer.pack(*values)
            sent = self.send(packed_data)
            self.__txcnt += 1
            self.__state=0
        if self.__state==2:
            print len(self.__queue)
            if len(self.__queue)> 0:
                data = list(self.__queue[0])
                data[2] = self.__state
                data[5] = self.__txcnt
                data[6] = self.__rxcnt
                print 'tx : ',len(data), data
                packed_data = self.__header_packer.pack(*list(data))
                sent = self.send(packed_data)
                self.__queue.pop(0)
                self.__txcnt += 1
                time.sleep(1)



if __name__ == "__main__":
    client = dbgClientCl('localhost', 8090)

    time.sleep(2)
    client.statReq(2)
    client.statReq(1)
    client.traceReq(1)

    asyncore.loop()        
