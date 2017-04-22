#ifndef UDPCLIENTSOCKET_H
#define UDPCLIENTSOCKET_H

#include "UDPSocket.h"

class UDPClientSocket : public UDPSocket // Inherit from UDPSocket
{
    public:
            UDPClientSocket ();// Constructor
            // Initialize socket to connect to an address and port
            bool initialize (char * _addr, int _myPort); 
            ~UDPClientSocket ( ); // Destructor
};

#endif // UDPCLIENTSOCKET_H
