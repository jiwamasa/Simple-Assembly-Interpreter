
#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "includes.h"


class TCPSocket // TCP Socket Class
{
 private:
  char remote_address[128]; // Remote IP address of socket connection.
  char my_address[128]; // The local IP address of the socket connection.
  struct sockaddr_in myAddr; // Socket connection structure.
  int port;   // TCP socket port number.
  int sock;   // Socket Descriptor.
  bool peerDisconnected; // A flag indicating the socket peer status.
  bool rshutdown;     // A flag indicating that a read shutdown was performed.
  bool wshutdown;     // A flag indicating that a write shutdown was performed.
  long stream_out_size;  // An integer representing the accumulated amount of data sent so far.
 public:
  // A constructor that allows creating a socket object after socket is created.
  TCPSocket (int _sock ,char * _address=NULL,int _port=0,int readBufferSize=65536,int writeBufferSize=65536);
  // A constructor that performs all the client connection initialization to a server socket
  TCPSocket (char * _address, int port,int readBufferSize=65536,int writeBufferSize=65536);
  char * getRemoteAddress();  // Selector returning remote IP address
  char * getMyAddress();      // Selector returning local IP address
  int readFromSocket (char * buffer, int maxBytes );  // Blocking read data from socket with an upperbound size
  // Blocking read data from socket with an upperbound size with a maximum timeout sec.milli-sec
  int readFromSocketWithTimeout (char * buffer, int maxBytes, long timeoutSec, long timeoutMilli);
  // Write data to a socket with a specified size.
  int writeToSocket (const char * buffer, long maxBytes );
  // Set the status of the socket peer as disconnected
  void setPeerDisconnected(bool value);
  // Check if the socket peer is connected
  bool isPeerDisconnected();
  // Perform a read shutdown
  void shutDownRead();
  // Perform a write shutdown
  void shutDownWrite();
  // Perform a bidirectional (Read/Write) shutdown
  void shutDown();
  // Get the amount of data written to a socket after invoking the writeToSocket method.
  long getStreamOutSize();
  ~TCPSocket ( ); // Destructor: shutdown the target socket and deallocate all its resources
};

#endif // TCPSOCKET_H

