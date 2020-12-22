/*
	TCP/IP client
*/ 


#include <stdio.h>

#if defined _WIN32

// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

#else
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#endif

#define SERVER_PORT 5060
//#define SERVER_IP_ADDRESS "172.17.17.23"
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SIZE 1024


int main()
{
	double timeTable [5] = {0};
	clock_t t;
	for(int i=0; i<5; i++)
	{
#if defined _WIN32
    // Windows requires initialization
    		WSADATA wsa;
    		if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    		{
        		printf("Failed. Error Code : %d",WSAGetLastError());
        		return 1;
    		}
#endif

     		int sock = socket(AF_INET, SOCK_STREAM, 0);

     		if(sock == -1)
    		{
        		printf("Could not create socket : %d" 
#if defined _WIN32
			,WSAGetLastError()
#else
			,errno
#endif
				);
    		}

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    		struct sockaddr_in serverAddress;
    		memset(&serverAddress, 0, sizeof(serverAddress));

    		serverAddress.sin_family = AF_INET;
    		serverAddress.sin_port = htons(SERVER_PORT);
		int rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &serverAddress.sin_addr);
		if (rval <= 0)
		{
			printf("inet_pton() failed");
			return -1;
		}

     // Make a connection to the server with socket SendingSocket.

     		if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)
     		{
			printf("connect() failed with error code : %d" 
#if defined _WIN32
			,WSAGetLastError()
#else
			,errno
#endif
	   		);
    		}

     		printf("connected to server\n");
     		// Sends some data to server
     		FILE *fp;
     		char *filename = "1gb.txt";
     
     		fp = fopen(filename, "w");
     		if(fp==NULL){
     			perror("error in read.");
     			exit(1);
     		}
     
     //char message[] = "Good morning, Vietnam\n";
     //int messageLen = strlen(filename) + 1;

		t = clock();
     		int bytesSent = send(sock, filename, SIZE, 0);

     		if (-1 == bytesSent)
     		{
			printf("send() failed with error code : %d" 
#if defined _WIN32
			,WSAGetLastError()
#else
			,errno
#endif
			);	
     		}
     		else if (0 == bytesSent)
     		{
			printf("peer has closed the TCP connection prior to send().\n");
     		}
     		else if (SIZE > bytesSent)
     		{
			printf("sent only %d bytes from the required %d.\n", bytesSent, SIZE);
     		}
     		else 
     		{
			printf("message was successfully sent. size = %d.\n", bytesSent);
     		}
     
     		char buffer[27];
     		int bytesRecieved = recv(sock, buffer, 27, 0);
     		if(bytesRecieved == -1){
     			printf("error has occurred.\n");
     			return(0);
     		}
     		else if(bytesRecieved == 0){
     			printf("socket closed.\n");
     		}
     		else if (bytesRecieved < 27)
     		{
			printf("recieved only %d bytes from the required %d.\n", bytesRecieved, 27);
     		}
     		else 
     		{
			printf("message was successfully recieved. size = %d.\n", bytesRecieved);
     		}
     		//This supposed to be in Mesure.
     		t=clock()-t;
     		timeTable[i]=((double)t)/CLOCKS_PER_SEC;	
     		printf("This file transfer took %lf.\n", timeTable[i]);
     

#if defined _WIN32
	 	Sleep(3000);
#else
	 	sleep(3);
#endif
	 

// TODO: All open clientSocket descriptors should be kept
    // in some container and closed as well.
#if defined _WIN32
    		closesocket(sock);
    		WSACleanup();
#else
    		close(sock);
#endif
	}
     	return 0;
}
