#include "UDPServerSocket.h"
UDPServerSocket::UDPServerSocket ( ):UDPSocket ()
{
}
bool UDPServerSocket::initialize (char * _addr,  int _myPort )
{
    struct hostent *hp; //A pointer to a host information structure        
    if ( _addr != NULL ) // If _peerAddr not equal to NULL then allocate memory and copy its value into peerAddress
    {
            myAddress = (char *) calloc (strlen(_addr)+10,sizeof(char));
            strcpy (myAddress,_addr);
    }
    else myAddress = NULL;
    myPort = _myPort; // set port

    // if sock is -1 then it was not initialized before, and so we can create a new socket
    if ( sock == -1) sock = socket(AF_INET,SOCK_DGRAM,0);
    if ( sock == -1 ) return false; // If sock == -1 after invoking the socket system call then and error occured
    // Setup myAddr
    sockaddr * myAddrCast = (sockaddr *) &myAddr ;
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons (myPort);
    if ( myAddress != NULL && strcmp (myAddress,"") != 0) // if peerAddress has a value then initialize peerAddr
    {
            hp = gethostbyname (myAddress);
            memcpy ((char *)&myAddr.sin_addr,(char *) hp->h_addr,hp->h_length);
    }
    else myAddr.sin_addr.s_addr = INADDR_ANY;
    // bind socket to address
    if (::bind (sock,myAddrCast,sizeof(sockaddr)) == 0 )
    { 
        // set socket read/write buffer.
        int buf_size= SOCKET_READ_BUFFER_SIZE;
        if ( setsockopt(sock, SOL_SOCKET, SO_RCVBUF,(char*) &buf_size,sizeof(int)) == -1) printf ("Error setting recieve buffer"); 
        if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF,(char*) &buf_size,sizeof(int)) == -1) printf ("Error setting send buffer");
        return true;
    }
    else return false;
}
UDPServerSocket::~UDPServerSocket ( )
{
}
