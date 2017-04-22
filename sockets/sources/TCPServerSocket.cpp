#include "TCPServerSocket.h"
// Constructor
TCPServerSocket::TCPServerSocket (const char * _address, int _port, int _backlog )
{
        // Allocate and initialize address 
	if ( _address != NULL && strcmp (_address,"") != 0)
	{
		address = (char *) calloc (strlen(_address)+1,sizeof(char));
		memset (address,0,strlen(_address)+1);
		strcpy (address,_address);
	}
	else address= NULL;
	port = _port; // Set port
	backlog=_backlog; // Set listen queue maximum size
}
bool TCPServerSocket::initializeSocket ( ) // Initialize server socket
{
        // Create a stream socket and return false on error
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false;
        // initialize serverAddr
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        if ( address != NULL ) // if server address is not NULL listen on specific interface
        {
            if (strcmp (address,"") !=0 ) serverAddr.sin_addr.s_addr = inet_addr(address);
            else serverAddr.sin_addr.s_addr = INADDR_ANY;
        } //else listen on all interfaces
        else serverAddr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(serverAddr.sin_zero), 8); // initialize last 8 bytes of serverAddr (Bug)
        // Disable the Nagle's algorithm
        int flag = 0;
        int result = setsockopt(sock,
                          IPPROTO_TCP,
                          TCP_NODELAY,
                          (char *) &flag,
                          sizeof(int));
        if ( result < 0 )
        {
	       close (sock);
      	       sock = -1;
       	       return false;
    	}
        // Enable socket reuse option.
         int sock_opt = 1;
         result = setsockopt(sock,
                           SOL_SOCKET,
                           SO_REUSEADDR,
                           (void *)&sock_opt,sizeof (sock_opt));
        if ( result < 0 )
 	{
 		close (sock);
 		sock = -1;
 		return false;
 	}
         // Bind the socket handler to the serverAddr
        if (::bind(sock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) == -1)
        {
                // Close socket and return false if bind failed
		close (sock);
		sock = -1;
		return false;
	}
        // Listen on the socket and configure the connection queue size
        if (listen(sock, backlog) == -1)
        {
		close (sock);
		sock = -1;
		return false;
	}
        return true;
}

 // Wait for a client connection. if timeoutSec and timeoutMilli are zeros the method will behave in a blocking mode
TCPSocket * TCPServerSocket::getConnection (int timeoutSec, int timeoutMilli,int readBufferSize,int writeBufferSize )
{
	socklen_t sin_size ;//= sizeof(struct sockaddr_in);
        int newsock = 0;
	if (timeoutSec==0 && timeoutMilli == 0 )// Blocking mode
	{
                // Wait for connection indefinitely
		newsock = accept(sock, (struct sockaddr *)&clientAddr,&sin_size);

	}
	else { // Set up time out timeval and file descriptors set
                fd_set fds; 
                struct timeval tv;
		tv.tv_sec = timeoutSec;
		tv.tv_usec = timeoutMilli;
		FD_ZERO(&fds);
		FD_SET(sock, &fds);
                // wait on activity on the socket for a timeout
		select(sock+1, &fds, NULL, NULL, &tv);
		if (FD_ISSET(sock, &fds)) // if sock is changed 
		{
                        // call accept on sock to get the pending connection
			newsock = accept(sock, (struct sockaddr *)&clientAddr,&sin_size);
		}
	}
        if ( newsock < 1 ) // if newsock is less than one then erroro
        {   // Print the error and return NULL
            perror("ERROR on accept");
            return NULL;
        }
        else{ // Else instantiate a TCPSocket object and return a pointer to it 
                TCPSocket * tcpSocket = new TCPSocket(newsock,(char *)inet_ntoa(clientAddr.sin_addr),clientAddr.sin_port,readBufferSize,writeBufferSize);
                return tcpSocket;
        }
        // If we are here then we return NULL
	return NULL;
}
TCPServerSocket::~TCPServerSocket ( ) // Destructor
{
	shutdown (sock,SHUT_RDWR);  // Shutdown the server read/write channels
	close (sock);              // Close socket descriptor
	if ( address != NULL ) free (address); // free the address buffer
}
