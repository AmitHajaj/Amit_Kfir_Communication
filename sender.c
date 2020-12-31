/*
	TCP/IP client
*/ 


#include <stdio.h>
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define SERVER_PORT 5060
//#define SERVER_IP_ADDRESS "172.17.17.23"
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SIZE (1024*1024)-1
#define BUF_SIZE 27
#define CHUNK_SIZE 256

double timeTable [5] = {0};
clock_t send_t;

double sendFile(int sock, int i, FILE *file){
	//send the file 10 times
	//after 5 times change CC to reno 

	//TODO when i==5 => change CC to reno

	//get clock's current time and send file annnd then check how many bytes was sent


	send_t = clock();
	ssize_t n;
	int len = SIZE;
	int bytesSent = send(sock, file, CHUNK_SIZE, 0);
    while (bytesSent<SIZE)
    {
        n = send(sock, file+CHUNK_SIZE, CHUNK_SIZE, 0);
        if (n <= 0){
			break;
		}
		bytesSent+= n;
        
        // len -= n;
    }

	// int bytesSent = send(sock, file, SIZE, 0);
	printf("send()\n");
	printf("%d\n", bytesSent);

	//the sent was faild
	if (bytesSent == -1)
	{
		printf("send() failed with error code : %d", errno);
	}

	//0 bytes was sent
	else if (bytesSent == 0)
	{
		printf("peer has closed the TCP connection prior to send().\n");
	}
	
	//too few bytes was sent
	else if (SIZE > bytesSent)
	{
		printf("sent only %d bytes from the required %d.\n", bytesSent, SIZE);
	}

	//the file was sent successfully
	else{
		printf("message was successfully sent. size = %d.\n", bytesSent);
	}

	//receiving accept message from server and check it
	char buffer[BUF_SIZE];
	int bytesRecieved = recv(sock, buffer, BUF_SIZE, 0);

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
	else if (bytesRecieved < BUF_SIZE)
	{
		printf("recieved only %d bytes from the required %d.\n", bytesRecieved, BUF_SIZE);
	}

	//message is valid
	else 
	{
		printf("message was successfully recieved. size = %d.\n", bytesRecieved);
	}

	//calculate time from sent to accept receiving
	send_t = clock()-send_t;
	timeTable[i]=(double)send_t / CLOCKS_PER_SEC;	
	printf("This file transfer took %lf.\n", timeTable[i]);

	sleep(3);
	
	return 0;
}

FILE* loadFile(){
	// load 1gb.txt to code
	FILE *fp;
	char *filename = "1gb.txt";
	int size=0;

	//open 1gb.txt for reading
	fp = fopen(filename, "r");
	if(fp==NULL){
		perror("error in read.");
		exit(1);
	}
	//to see if the file read fully.
	fseek(fp, 0, 2);
	size = ftell(fp);

	return fp;
}

int main()
{
	//create the socket and check if valid
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
	{
		printf("Could not create socket : %d", errno);
	}

	//initialize and clean serverAddress that will hold the socket's address
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));

	//set serverAddress structure
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(SERVER_PORT);

	//check conversion of IP address to binary 
	int rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &serverAddress.sin_addr);
	if (rval <= 0)
	{
		printf("inet_pton() failed");
		return -1;
	}

	// Make a connection to Measure and check if connection made
	int connectStatus = connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	if (connectStatus== -1)
	{
		printf("connect() failed with error code : %d", errno);
	}else{
		printf("connected to server\n");
	}

	//load file
	FILE *file = loadFile();

	//send file 10 times
	int ccType = 0;
	for(int i=0; i<10; i++)
	{
		//change the TCP_CONGESTION algoritm to reno.
		if(i==5){
			char buf[256];
			socklen_t len;
			len = sizeof(buf); 
			if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) { 
				perror("getsockopt");
				return -1;
			}

			printf("Current: %s\n", buf); 



			strcpy(buf, "reno"); 
			len = strlen(buf);
			if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
				perror("setsockopt"); 
				return -1;
			}
			len = sizeof(buf); 
			if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
				perror("getsockopt"); 
				return -1; 
			} 
			printf("New: %s\n", buf); 

		}
		sendFile(sock, i, file);
		printf("file was sent %d times \n", i+1);
	}

// TODO: All open clientSocket descriptors should be kept
// in some container and closed as well.
	close(sock);
	
	return 0;
}
