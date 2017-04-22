#ifndef UDPSERVERSOCKET_H
#define UDPSERVERSOCKET_H

#include "UDPSocket.h"

class UDPServerSocket: public UDPSocket
{

public:
	UDPServerSocket (); // Constructor
        // Initialize socket to reseve connections and data on _addr and _myport
	bool initialize (char * _addr, int _myPort);
	~UDPServerSocket ( ); // Destructor
};

#endif // UDPSERVERSOCKET_H
