#ifndef TCPSERVERSOCKET_H
#define TCPSERVERSOCKET_H

#include "includes.h"
#include "TCPSocket.h"


class TCPServerSocket // TCP Server Socket
{
	private:
		int sock;   // Socket Handler
		struct sockaddr_in serverAddr;  // Server Socket Address Structure
		struct sockaddr_in clientAddr;  // Client Socket Address Structure
		char * address; // Local address for the server socket to bind to
		int port; // Local port for the server socket to bind to
		int backlog; // Maximum length of the queue fo pending connections.
	public:
                // Constructor
		TCPServerSocket (const char * _address, int _port, int _backlog );
		bool initializeSocket (); // Initailize server socket
                // Wait for a client connection and return a TCPSocket object that represents the client
		TCPSocket * getConnection (int timeoutSec=0, int timeoutMilli=0,int readBufferSize=10*1024*1024,int writeBufferSize=10*1024*1024);
		~TCPServerSocket ( ); // Destructor
};

#endif // TCPSERVERSOCKET_H
