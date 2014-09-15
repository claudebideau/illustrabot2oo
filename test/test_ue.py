#!/usr/bin/env python

import sys
import time
import binascii
import socket
import struct

import random
import math


G_maxV =32
G_noise=64

def AWGN_generator(NOISE_POWER=64):
    # Generates additive white Gaussian Noise samples with zero mean and a standard deviation of 1.
    p=1
    
    while p>0 :
        temp2 = random.random()
        if temp2>=0.5: 
            p = 1
        else:
            p =-1

    temp1 = math.cos( ( 2.0 * math.pi ) * random.random() );
    result = math.sqrt( -2.0 * math.log( temp2 ) ) * temp1;
    return int(result*NOISE_POWER)

def makeSinusNoise(nb_sample= 32, start_angle=0.0, max_amplitude= 1024, noise = ):
    table = [ int(math.cos(start_angle+2*math.pi *2*i/G_maxV)*max_amplitude) for i in range(nb_sample)]
    tablenoise = [ v + AWGN_generator(G_noise) for v in table]
        
    return tablenoise


class testSoc:
    """
    typedef struct message {
       uint8_t   version;   /**< protocol version */
       uint8_t   size;      /**< message size */
       msgType_t type;
          struct {
             uint8_t b1;    /**< button 1 */
             uint8_t b2;    /**< button 2 */
             uint8_t b3;    /**< button 3 */
             uint8_t b4;    /**< button 4 */
             int32_t azimuth;   /**< axis X * 100 */
             int32_t pitch;     /**< axis Y * 100 */
             int32_t roll;      /**< axis Z * 100 */
          } android;
    } message_t;
    """
    
    
    def __init__( self, ip="localhost", port=8089, angle=(0.0,math.pi/2,0.1)) :
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__server_address = (ip, port)
        self.__sock.connect(self.__server_address)
        self.__sample = [ makeSinusNoise(angle[0]), makeSinusNoise(angle[1]), makeSinusNoise(angle[2])]
        self.__packer = struct.Struct('B B B B B B B B i i i i')
        self.__idx = 0
        
    def sendData(self):
        try:
            sys.stdout.write(".")
            # print len(self.__sample[0]),len(self.__sample[1]),len(self.__sample[2])
            if self.__idx >= len(self.__sample[0]): raise "Ended"
            values = (1, 20, 3, 0, 0, 0, 0, 0, self.__sample[0][self.__idx], self.__sample[1][self.__idx], self.__sample[2][self.__idx], 0)
            packed_data = self.__packer.pack(*values)
            self.__sock.sendall(packed_data)
            self.__idx += 1
            # print self.__idx
        except:
            print "Unexpected error:", sys.exc_info()[0]
            print >>sys.stderr, 'closing socket'
            self.__sock.close()
            return False
        return True
        
if __name__ == "__main__":
    random.seed(0)
    sock1 = testSoc()
    sock2 = testSoc(angle=(0.1,0.0,0.3))
    sended=True
    while sended:
        sended  = sock1.sendData()
        sended &= sock2.sendData()
        time.sleep(1)

#for i in range(4):
#    try:
#        
#        # Send data
#        values = (1, 20, 3, 0, 0, 0, 0, 0, 100, 200, 400, 0)
#        packed_data = packer.pack(*values)
#        sock1.sendall(packed_data)
#        values = (1, 20, 3, 0, 0, 0, 0, 0, 128, 256, 512, 0)
#        packed_data = packer.pack(*values)
#        sock1.sendall(packed_data)
#        time.sleep(1)
#        
#        # message = 'This is the message'
#        # print >>sys.stderr, 'sending "%s"' % message
#        # time.sleep(1)
#        # message = 'It will be repeated.'
#        # print >>sys.stderr, 'sending "%s"' % message
#        # sock1.sendall(message)
#        # time.sleep(1)
#
#        # Look for the response
#        # amount_received = 0
#        # amount_expected = len(message)
#        
#        # while amount_received < amount_expected:
#            # data = sock1.recv(16)
#            # amount_received += len(data)
#            # print >>sys.stderr, 'received "%s"' % data
#
#    finally:
#        print >>sys.stderr, 'closing socket'
#
#sock1.close()
        
