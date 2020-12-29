/*
    TCP/IP-server
*/

#include<stdio.h>

#if defined _WIN32
#include<winsock2.h>   //winsock2 should be before windows
#pragma comment(lib,"ws2_32.lib")
#else
// Linux and other UNIXes
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#endif

#define SERVER_PORT 5060  //The port that the server listens
#define SIZE 1024*1024        //Size of the file.
#define CHUNK_SIZE 256
  
double timeTable [10] = {0};
double avg=0;

int main()
{
	//initialize and clean sockaddr_in that will hold the socket's address
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	clock_t receiving_t;

	// on linux to prevent crash on closing socket
	signal(SIGPIPE, SIG_IGN); 
      
    // Open the listening socket (server)
    int listener = socket(AF_INET , SOCK_STREAM , 0 );  
    if( listener == -1)
    {
        printf("Could not create listening socket : %d" ,errno);
    }else{
		printf("Listening soucket was create successfully");
	}

	//clean serverAddress that will hold the listening socket's address
	memset(&serverAddress, 0, sizeof(serverAddress));

	//set sockaddr_in structure
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  //network order

	// allow reuse of socket if was already in use
    int yes = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
         printf("setsockopt() failed with error code : %d" ,errno);
    }

    // Bind the socket to the port with any IP at this port and check it
    if (bind(listener, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code : %d" , errno);
		// TODO: close the socket
        return -1;
    }else{
		printf("Bind() success\n");
	}

	//TODO maybe change 500 to 10?
    //create a connection requests queue from clients in size of 500
    if (listen(listener, 500) == -1)
    {
		printf("listen() failed with error code : %d", errno);
		// TODO: close the socket
        return -1;
    }
      
    //Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");
      
    socklen_t clientAddressLen = sizeof(clientAddress);

	int i = 0;
	int bytesCnt;
	char buffer[CHUNK_SIZE];
	clientAddressLen = sizeof(clientAddress);
    int clientSocket = accept(listener, (struct sockaddr *)&clientAddress, &clientAddressLen);
	//TODO maybe while(1)?
    while (i<10)
    {
		//TODO change to reno
		
		bytesCnt = 0;
		//accept first request from requests queue

		
    	if (clientSocket == -1)
    	{
           printf("listen failed with error code : %d", errno);
		   // TODO: close the sockets
           return -1;
    	}else{
			printf("The connection was successful\n");
		}

		//TODO maybe change buf to pointer?
        //TODO maybe clean buffer
		//receiving file from sender and check it
    	
		memset(buffer, 0, CHUNK_SIZE);
     	int bytesRecieved = recv(clientSocket, buffer, CHUNK_SIZE, 0);
		bytesCnt += bytesRecieved;

        //Get all packets for this message.
        while(bytesCnt < SIZE){
            memset(buffer, 0, CHUNK_SIZE);
            bytesRecieved = recv(clientSocket, buffer, CHUNK_SIZE, 0);
            bytesCnt += bytesRecieved;
        }

     	//error in messege receiving
		if(bytesRecieved == -1){
    		printf("error has occurred.\n");
    		return(0);
     	}

		//the message is empty
    	else if(bytesRecieved == 0){
			printf("socket closed.\n");
     	}

		//the message was too short
     	// else if (bytesRecieved < SIZE)
     	// {
		// 	printf("recieved only %d bytes from the required %d.\n", bytesRecieved, SIZE);
     	// }

		//message is valid
     	else if(bytesCnt == SIZE)
     	{
			printf("message was successfully recieved. size = %d.\n", bytesCnt);
     	}
    
     	receiving_t =clock()-receiving_t;
     	timeTable[i%5] =(double)receiving_t / CLOCKS_PER_SEC;
		
     	printf("This transfer took - %lf seconds.\n", timeTable[i%5]);
      
    	printf("A new client connection accepted\n");
    	
		//TODO change message

    	//Reply to client
    	char message[] = "Welcome to our TCP-server\n";
        int messageLen = strlen(message) + 1;

    	int bytesSent = send(clientSocket, message, messageLen, 0);
		if (-1 == bytesSent)
		{
			printf("send() failed with error code : %d", errno);
		}
		else if (0 == bytesSent)
		{
		   printf("peer has closed the TCP connection prior to send().\n");
		}
		else if (messageLen > bytesSent)
		{
		   printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
		}
		else 
		{
		   printf("message was successfully sent. size= %d.\n", bytesSent);
		   if(i==9 || i==4){
				for(int j=0; j<5; j++){
					avg += timeTable[j];
				}
				avg = avg/5.0;
				printf("avg is: %lf.\n", avg);
			}
			
		}

		i++;
	}

    //TODO maybe close all clientSockets
	close(clientSocket);
	close(listener);
    return 0;
}
