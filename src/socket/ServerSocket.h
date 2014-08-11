// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"


class ServerSocket : private Socket
{
 public:

  ServerSocket ( int port );
  ServerSocket (){};
  virtual ~ServerSocket();

  // Data Transmission
  bool send ( const std::string &) const;
  bool send ( void * buf, ssize_t size ) const;
  
  int recv ( std::string& ) const;
  int recv ( void * buf ) const;

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;

  void accept ( ServerSocket& );

};


#endif
