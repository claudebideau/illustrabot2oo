#include <iostream>
#include "ServerSocket.h"
#include "SocketException.h"
#include <string>
#include "proto.h"
#include <stdio.h>

/** Display a received message from the client as hex bytes
 * @param msg the message to dump
 */
//void hex_dump_message (std::string &msg)
void hex_dump_message (void * msg)
{
//   char* ptr = (char*)msg.c_str();
   char* ptr = (char*)msg;
   int i;

   for (i=0;i < sizeof(message_t);i++) {
      printf ("%02X ",(uint8_t) ptr[i]);
   }
   printf("\n");
}// hex_dump_message



int main ( int argc, char * argv[] )
{
    std::cout << "running....\n";

    try
    {
      // Create the socket
      ServerSocket server ( 9099 );

        while ( true )
        {

            ServerSocket new_sock;
            server.accept ( new_sock );
            std::cout << "accept"<<std::endl;

            try
            {
                while ( true )
                {
                    message_t data;

                    new_sock.recv((void*)&data);
                    hex_dump_message((void*)&data);
                    //new_sock >> data;
                    // new_sock << data;
                    new_sock.send((void*)&data, sizeof(message_t));
                }
            }
            catch ( SocketException& ) {}

        }
    }
    catch ( SocketException& e )
    {
        std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }

  return 0;
}
