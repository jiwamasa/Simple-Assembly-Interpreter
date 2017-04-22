#include "UDPClientSocket.h"

UDPClientSocket::UDPClientSocket ( ) : UDPSocket () // Invoke UDP Socket comstrutor
{
    
}
bool UDPClientSocket::initialize (char * _peerAddr,  int _peerPort )
{
    // if sock is -1 then it was not initialized before, and so we can create a new socket
    if ( sock == -1) sock = socket(AF_INET,SOCK_DGRAM,0);
    if ( sock == -1 ) return false; // If sock == -1 after invoking the socket system call then and error occured
    struct hostent *hp; //A pointer to a host information structure
    if ( _peerAddr != NULL ) // If _peerAddr not equal to NULL then allocate memory and copy its value into peerAddress
    {
        peerAddress = (char *) calloc (strlen(_peerAddr)+1,sizeof(char));
        strcpy (peerAddress,_peerAddr);
    }
    else peerAddress = NULL;
    peerPort = _peerPort; // Set peerPOrt
    // Setup peerAddr
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons (peerPort);
    if ( peerAddress != NULL && strcmp (peerAddress,"") != 0) // if peerAddress has a value then initialize peerAddr
    {
        hp = gethostbyname (peerAddress);
        memcpy ((char *)&peerAddr.sin_addr,(char *) hp->h_addr,hp->h_length);
    }
    else enabled = false;
    // set socket read/write buffer.
    int buf_size= SOCKET_READ_BUFFER_SIZE;
    if ( setsockopt(sock, SOL_SOCKET, SO_RCVBUF,(char*) &buf_size,sizeof(int)) == -1) printf ("Error setting recieve buffer"); 
    if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF,(char*) &buf_size,sizeof(int)) == -1) printf ("Error setting send buffer");
    return true;
}

UDPClientSocket::~UDPClientSocket ( )
{
}
