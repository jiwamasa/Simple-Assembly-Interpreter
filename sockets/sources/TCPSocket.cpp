#include "TCPSocket.h"
// Ths constructor is used when the socket is already created and established prior to object instantiation
TCPSocket::TCPSocket (int _sock,char * _address, int _port ,int readBufferSize,int writeBufferSize)
{
    stream_out_size = 0;    // Initialize stream_out_size to zero since nothing is sent yet
    sock = _sock;           // Set socket descriptor already created before instantiation
    peerDisconnected = false;   // Set peerDisconnected to false
    memset (remote_address ,0 , 128);   // initialize remote_address
    memset (my_address ,0 , 128);       // initialize my address
    
    if ( _address != NULL) strcpy(remote_address,_address); // If _address is not NULL copy it to remote_address data member
    port = _port; // Set the port number
    // Setting receive and send socket buffers if the parameters readBufferSize and writeBufferSize are nit set to -1 respectively
    if ( readBufferSize != -1 && setsockopt(sock, SOL_SOCKET, SO_RCVBUF,(void*) &readBufferSize,sizeof(int)) == -1)
        printf ("Error setting receive buffer\n");
    if ( writeBufferSize != -1 &&  setsockopt(sock, SOL_SOCKET, SO_SNDBUF,(void*) &writeBufferSize,sizeof(int)) == -1)
        printf ("Error setting send buffer\n");
    
    // Disable the Nagle's algorithm
    int flag = 0;
    setsockopt(sock,
                IPPROTO_TCP,
                TCP_NODELAY,
                (char *) &flag,
                sizeof(int));
    // Get size of the sockaddr structure
    socklen_t my_sock_size = sizeof(struct sockaddr);
    getsockname (sock,(struct sockaddr *)&myAddr,&my_sock_size); // Get the current address of the socket
    strcpy(my_address , inet_ntoa(myAddr.sin_addr)); // Convert Internet address to a string representing IP address
    rshutdown = false; // Set rshutdown to false.
    wshutdown= false; // Set wshutdown to false.
}
// This constructor is invoked to initialize and connect the socket to a server socket using remote address and port number
TCPSocket::TCPSocket (char * _address, int port ,int readBufferSize,int writeBufferSize)
{
    // Initialize some data members
    memset (remote_address, 0, 128);
    memset (my_address, 0, 128);
    stream_out_size = 0;
    peerDisconnected = false;
    strcpy(remote_address, _address);

    // Create a stream socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr; // Define a server socket structure
    struct hostent *server; // A pointer to a structure to store host specific information about the server
    server = gethostbyname(remote_address);
    // Check if server information is available
    if ( server == NULL ||  server->h_addr == NULL) 
        printf ("Cannot retrieve server address information\n");
    else { // initialize serve_addr and set it up
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(port); // Handling network byte order
        // Disable Nagle's algorithm
        int flag = 0;
        setsockopt(sock,
                    IPPROTO_TCP,
                    TCP_NODELAY,
                    (char *) &flag,
                    sizeof(int));
        // Setting receive and send socket buffers if the parameters readBufferSize and writeBufferSize are nit set to -1 respectively
        if ( readBufferSize != -1 && setsockopt(sock, SOL_SOCKET, SO_RCVBUF,(void*) &readBufferSize,sizeof(int)) == -1)
            printf ("Error setting receive buffer\n");
        if ( writeBufferSize != -1 &&  setsockopt(sock, SOL_SOCKET, SO_SNDBUF,(void*) &writeBufferSize,sizeof(int)) == -1)
            printf ("Error setting send buffer\n");
        // Connect to remote server
        if (connect(sock,(sockaddr *)&serv_addr,sizeof(serv_addr)) == -1 )  { // if cannot connect
            perror ("Connection error\n");  // Print error using perror
            peerDisconnected = true; // set peerDisconnected to true
            rshutdown = true;
            wshutdown= true;            
        }
        else { // Get local socket information and loaded into data members
            socklen_t my_sock_size = sizeof(struct sockaddr);
            getsockname (sock,(struct sockaddr *)&myAddr,&my_sock_size);
            strcpy(my_address , inet_ntoa(myAddr.sin_addr));
            rshutdown = false; // Set rshutdown to false.
            wshutdown= false;  // Set wshutdown to false.
        }
    }
}
char * TCPSocket::getRemoteAddress() { return remote_address; } // Selector returning the remote address

char * TCPSocket::getMyAddress() {return my_address;} // Selector returning the socket local address

int TCPSocket::readFromSocket (char * buffer, int maxBytes ) { // Blocking read data operation from socket
    if ( buffer != NULL ) {
            memset (buffer,0,maxBytes); // initialize buffer
            int read = recv (sock,buffer,maxBytes,0); // invoke recv system call
            return read; // return the number of bytes read
    }else return -1; // return -1 on error
}

int TCPSocket::readFromSocketWithTimeout (char * buffer, int maxBytes, long timeoutSec, long timeoutMilli)
{
    fd_set fds; // A file descriptors set
    int read =-1;  // A variable for storing the amount of data read and initialized to -1
    struct timeval tv; // a timeval structure
    // Initializing the tv variable with time out parameters
    tv.tv_sec = timeoutSec;
    tv.tv_usec = timeoutMilli;
    // Initializing the file descriptors set and adding the socket descriptor to it
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    // Using select to sleep on the socket and file descriptor
    int select_result =  select(sock+1, &fds, NULL, NULL, &tv);
    if ( select_result >= 0 ) // if select results is bigger than 0 this means that data is available
    {
        if (FD_ISSET(sock, &fds))
        {
            read = recv (sock,buffer,maxBytes,0); // read data from socket
            // if any shutdown is performed return -1
            if ( rshutdown && wshutdown && read == 0 ) return -1;
            else return read; // else return the number if bytes read
        }
        else if (select_result == 0 ) return 0; // Timedout
    } else perror("Error reading from socket: "); // Else print error
    return read;
}
int TCPSocket::writeToSocket (const char * buffer, long maxBytes ) //Write data to socket
{
    if ( buffer != NULL ) { // If buffer is not NULL
        int wrote = send (sock,buffer,maxBytes,0); // user send system call to write data to socket
        stream_out_size += wrote; // add  to stream_out_size the amount of data written
        if ( wrote != maxBytes) perror("socket error"); // if wrote != maxBytes then an error occurred during writing
        return wrote; // return the number of bytes written
    } else return -1; // else return 01
}
long TCPSocket::getStreamOutSize() {return stream_out_size;} // Selector returning number of bytes written so far
void TCPSocket::setPeerDisconnected(bool value){peerDisconnected = value;} // Modifier setting peerDisconnected Flag
bool TCPSocket::isPeerDisconnected() {return peerDisconnected;} // Check the peerDisconnected flag
void TCPSocket::shutDownRead ( ) // Shutdown the read channel of the socket
{
    if ( !rshutdown)
    {
        shutdown (sock,SHUT_RD);
        rshutdown = true;
    }
}
void TCPSocket::shutDownWrite ( ) // Shutdown the write channel of the socket
{
    if ( !wshutdown)
    {
        shutdown (sock,SHUT_WR);
        wshutdown = true;
    }
}
void TCPSocket::shutDown ( ) // Shutdown socket read and write channels
{
        shutDownRead ();
	shutDownWrite();
}

TCPSocket::~TCPSocket ( ) // Descriptor
{
        shutDown(); // Shutdown
        close (sock); // Close socket descriptor
}
