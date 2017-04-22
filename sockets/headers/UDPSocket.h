#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "includes.h"

class UDPSocket
{
    protected:
	int sock;   // Socket Handler
	sockaddr_in myAddr; // Socket Address Structure representing local end of the socket
	sockaddr_in peerAddr; // Socket Address Structure representing remote end of the socket
	char * myAddress;   // Local IP address
	char * peerAddress; // Remote IP address
	char * filterAddress; // An address used to filter packets and only accept packets from it 
	int myPort;     // Local port of the socket
	int peerPort;   // Report port of the socket.
	bool enabled;   // A flag representing if the socket is enabled or disabled
    public:
	UDPSocket ();   // Constructor
	void setFilterAddress (char * _filterAddress); // Modifier for setting the filter address
	char * getFilterAddress ();     // Select for returning the filter address
        // Pure virtual method for initialization to be extended by the client and server classes
	virtual bool initialize (char * _addr, int _myPort) = 0;
        // Write to a socket a buffer with a specific size (Need to fit into UDP Packet)
	int writeToSocket (char * buffer,  int maxBytes ); 
        // Write to a socket a buffer with a specific size and wait for a number of micro seconds before returning (Need to fit into UDP Packet)
	int writeToSocketAndWait (char * buffer, int  maxBytes,int microSec );
        // Check if socket has data from the filter address and return right away
	int readFromSocketWithNoBlock (char * buffer, int  maxBytes );
        // Block for a maximum of timeout for a packet from the filter address
	int readFromSocketWithTimeout (char * buffer, int maxBytes, int timeoutSec, int timeoutMilli);
        // Block reading a packet from the filter address
	int readFromSocketWithBlock (char * buffer,  int maxBytes );
        // Check if socket has data from any source address and return right away
	int readSocketWithNoBlock (char * buffer, int  maxBytes );
        // Block for a maximum of timeout for a packet from any address
	int readSocketWithTimeout (char * buffer, int maxBytes, int timeoutSec, int timeoutMilli);
        // Block reading a packet from any address
	int readSocketWithBlock (char * buffer,  int maxBytes );
	char * getMyAddress();  // Get the address that the socket is bound to
	char * getPeerAddress(); // Get the remote address
	int getMyPort (); // Selector returning the local port number of the socket
	int getPeerPort (); // Selector returning the remote port number of the socket
	void enable(); // Enable socket
	void disable(); // Disable socket
	bool isEnabled();   // Check if socket is enabled
	char * getMyLastAddress(); //Get last address a packet received on
	char * getLastPeerAddress(); // Get the remote address of the last packet received
	int getLastPeerPort (); // Get the remote port of the last packet received
	int getSocketHandler(); // Return the socket handler (sock)
	virtual ~UDPSocket ( ); // Destructor
};

#endif // UDPSOCKET_H
