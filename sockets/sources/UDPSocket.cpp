#include "UDPSocket.h"
UDPSocket::UDPSocket ( ) // Constructor
{
    // Initialize all data memvbers to unusable values and set socket to be disabled
    myAddress = NULL;
    peerAddress = NULL;
    myPort = 0;
    peerPort = 0;
    sock = -1;
    filterAddress=NULL;
    enabled =false;
}
// Set the filter address
void UDPSocket::setFilterAddress (char * _filterAddress)
{
    // If the filter address is already allocted then deallocate it
    if ( filterAddress != NULL ) free (filterAddress);
    if ( _filterAddress != NULL ) // if the provided filter address is not NULL
    {
            // Allocate space for the filter address and copy to it the provided filter address parameter
            filterAddress = (char *) calloc (strlen(_filterAddress)+5,sizeof(char));
            memset (filterAddress,0,strlen(_filterAddress)+5);
            strcpy (filterAddress,_filterAddress);
    } else filterAddress = NULL; // else set filter address to NULL
}
//Write data to socket as a packet. maxBytes need to fit into a packet
int UDPSocket::writeToSocket (char * buffer,  int maxBytes )
{
    if ( !isEnabled() ) // If socket is not enabled return -1 and print an error message
    {
        printf ("Write: This socket is not enabled (writeToSocket): %d\n",getPeerPort());
        return -1;
    }
    // Send packet to the current peerAddress (set by the client or the server classes) and return the number of bytes returned by the sendto
    return sendto (sock,buffer,maxBytes,0,(sockaddr * ) &peerAddr,sizeof(struct sockaddr_in)); 

}
//Write data to socket as a packet and wait for micorSec before returning. maxBytes need to fit into a packet
int UDPSocket::writeToSocketAndWait (char * buffer,  int maxBytes,int microSec )
{
    if ( !isEnabled() ) // If socket is not enabled return -1 and print an error message
    {
        printf ("Write: This socket is not enabled (writeToSocketAndWait)\n");
        return -1;
    }
    int wrote = writeToSocket (buffer,maxBytes); // Invoke write to socket
    if (wrote > 0 && microSec > 0) usleep (microSec); // Sleep for micorSec
    return wrote; //return the number of bytes sent
}
// Check if socket has data from the filter address and return right away
int UDPSocket::readFromSocketWithNoBlock (char * buffer,  int maxBytes )
{
    if ( !isEnabled() ) return -1; // Return -1 if socket is not enabled
    int read  = 0 ; // Initialize number of read bytes to zero
    socklen_t size;
    // Read packet for socket and load the sender address details into peerAddr. Notice the MSG_DONTWAIT flag that makes the recvfrom return right away
    read = recvfrom (sock,buffer,maxBytes,MSG_DONTWAIT,(sockaddr * ) &peerAddr,&size);
    if ( read > 0) // If bytes read
    {
        if ( filterAddress != NULL ) // Check if the address filter is not NULL
        {
            // Check if the sender address is equal to the filter address
            if ( strcmp ( filterAddress,inet_ntoa (peerAddr.sin_addr)) != 0 )
            {   // If not reset read and the buffer
                memset ( buffer,0,maxBytes);
                read = 0;
            }
        } // If NULL it will accept a packet from any source address
    } // else error with receive
    else memset ( buffer,0,maxBytes);
    return read;  // Return number of bytes read
}
// Block for a maximum of timeout for a packet from the filter address
int UDPSocket::readFromSocketWithTimeout (char * buffer, int maxBytes, int timeoutSec, int timeoutMilli)
{
    if ( !isEnabled() ) return -1; // return -1 if socket is not enabled
    int read = 0;
    socklen_t size;
    // Set timeval and file descriptors set and add sock to it 
    fd_set fds;
    struct timeval tv;
    tv.tv_sec = timeoutSec;
    tv.tv_usec = timeoutMilli;
    FD_ZERO(&fds);
    FD_SET(sock, &fds); 
    memset ( buffer,0,maxBytes);
    select(sock+1, &fds, NULL, NULL, &tv); //Block for activity
    if (FD_ISSET(sock, &fds)) // If activity occurs fetch packet
    {
        // Read packet for socket and load the sender address details into peerAddr. If we are here we are sure that there is an available packet
        read = recvfrom(sock, buffer, maxBytes, 0,(sockaddr * ) &peerAddr,&size);
        if ( read > 0 )
        {
            if ( filterAddress != NULL ) // Check if the address filter is not NULL
            {
                // Check if the sender address is equal to the filter address
                if ( strcmp ( filterAddress,inet_ntoa (peerAddr.sin_addr)) != 0 )
                {
                    memset ( buffer,0,maxBytes);
                    read = 0;
                }
            } // If NULL it will accept a packet from any source address
        }
    }  // Else an error occurred and thus we set read to -1
    else read = -1;
    return read;
}
// Block reading a packet from the filter address
int UDPSocket::readFromSocketWithBlock (char * buffer,  int maxBytes )
{
    if ( !isEnabled() ) return -1; // return -1 if socket is not enabled    
    int read  = 0 ;
    socklen_t size;
    memset ( buffer,0,maxBytes);
    // block on recvfrom to read a packet and load sender information info peerAddr
    read = recvfrom(sock, buffer, maxBytes, 0,(sockaddr * ) &peerAddr,&size);
    if ( filterAddress != NULL && read > 0)
    {
        // Check if the sender address is equal to the filter address
        if ( strcmp ( filterAddress,inet_ntoa (peerAddr.sin_addr)) != 0 )
        {
            memset ( buffer,0,maxBytes);
            read = 0;
        }
    } // If NULL it will accept a packet from any source address
    else read = -1;
    return read;
}
// Check if socket has data from any source address and return right away
int UDPSocket::readSocketWithNoBlock (char * buffer,  int maxBytes )
{
    int read  = 0 ; // set number of read bytes to zero
    memset ( buffer,0,maxBytes); // Clear  buffer
    if ( !isEnabled()) read = -1; // If not enabled return -1
    // Else read packet for socket. Notice the MSG_DONTWAIT flag that makes the recv return right away
    else read = recv (sock,buffer,maxBytes,MSG_DONTWAIT);
    return read;
}
// Block for a maximum of timeout for a packet from any address
int UDPSocket::readSocketWithTimeout (char * buffer, int maxBytes, int timeoutSec, int timeoutMilli)
{
    int read = 0; // Set number of read bytes to zero
    memset ( buffer,0,maxBytes);
    if ( !isEnabled()) read = -1; // If socket is not enabled clear buffer and return -1;
    else
    {
        // Set timeval and file descriptors set and add sock to it 
        fd_set fds;
        struct timeval tv;
        tv.tv_sec = timeoutSec;
        tv.tv_usec = timeoutMilli;
        FD_ZERO(&fds);
        FD_SET(sock, &fds); 
        // Wait for activity on the socket.
        select(sock+1, &fds, NULL, NULL, &tv);
        if (FD_ISSET(sock, &fds)) // If activity occurs fetch packet
            read = recv(sock, buffer, maxBytes, 0);
        else read = -1;
    }
    return read;
}
// Block reading a packet from any address
int UDPSocket::readSocketWithBlock (char * buffer,  int maxBytes )
{
    int read  = 0 ; // Set number of bytes read to 0
    memset ( buffer,0,maxBytes); // clear buffer
    if ( !isEnabled()) read = -1; // if not enabled return -1
    else read = recv(sock, buffer, maxBytes, 0); // read a packet from the socket
    return read;
}
int UDPSocket::getMyPort (){return myPort;} // Return the local port of the socket
int UDPSocket::getPeerPort (){return peerPort;} // Return the remote port of the socket
void UDPSocket::enable() {enabled = true;} // Set enabled to true
void UDPSocket::disable(){enabled = false;} // Set enabled to false
bool UDPSocket::isEnabled(){return enabled;} // Return the value of the enabled flag
// Return the local IP address configured by the Client/Server Classes
char * UDPSocket::getMyAddress() { return myAddress;}
// Return the local IP address that received the last packet
char * UDPSocket::getMyLastAddress() {
        // convert the local socket address to an IP string and return it
	return inet_ntoa(myAddr.sin_addr);
}

// Return the remote address set by the Client Class 
char * UDPSocket::getPeerAddress () {return peerAddress;}

// Return remote IP address of the last packet received
char * UDPSocket::getLastPeerAddress () { 
        // convert the remote socket address to an IP string and return it
	return inet_ntoa(peerAddr.sin_addr);
}
int UDPSocket::getLastPeerPort () { // Return remote port number of the socket
        // Use the network to host short convertor to get the peer port number
	return ntohs(peerAddr.sin_port);
}
char * UDPSocket::getFilterAddress () { return filterAddress; } // Return filter address
int UDPSocket::getSocketHandler() { return sock;} // Return the socket handler

UDPSocket::~UDPSocket ( ) // Destructor 
{
        // Free myAddress and peerAddress if allocated
	if ( myAddress != NULL ) free (myAddress);
	if ( peerAddress != NULL ) free (peerAddress);
	if ( sock != -1 ) close (sock); // Close socket if it was created
}
