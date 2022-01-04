// Assignment 3 – Network Security – Winter 2021 - Milan Jovanovic
//
//
//  This file contains all items needed for a client, such as the connection to packetization. 
//The client can request rest operations on articles from the server
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <ws2tcpip.h>
#define PORT 5000
#define MAXLIST 15
#define MAXAUTHORLENG 15
#define MAXTOPICLENG 45
#define MAXPOSTLENG 500


void constructRequest(char* requestType,char* data)
{
	//intializing variables for user input
	char choice[MAXLIST];
	char inputTopic[MAXTOPICLENG];
	char inputAuthor[MAXAUTHORLENG];
	char inputPost[MAXPOSTLENG];
	
	//if the request has POST in it
	if (strcmp(requestType, "POST") == 0)
	{
		//get data from user
		printf("Enter Author:");
		gets(inputAuthor);

		printf("Enter Topic:");
		gets(inputTopic);

		printf("Enter Post:");
		gets(inputPost);

		//creating request
		strcat(data, "POST  http://127.0.0.1:5000/posts  HTTP/1.1\n\n");

		strcat(data, inputAuthor);
		strcat(data, "_");

		strcat(data, inputTopic);
		strcat(data, "_");

		strcat(data, inputPost);
		strcat(data, "_");
	}
	//if the request has GET in it
	else if(strcmp(requestType, "GET") == 0)
	{
		printf("Enter post number:");
		gets(choice);

		//creating request
		strcat(data, "GET http://127.0.0.1:5000/posts/"); 

		strcat(data, choice);

		strcat(data, "  HTTP / 1.1\n\n");

	}
	//if the request has DELETE in it
	else if (strcmp(requestType, "DELETE") == 0)
	{
		printf("Enter post number:");
		gets(choice);

		//creating request
		strcat(data, "DELETE http://127.0.0.1:5000/posts/");

		strcat(data, choice);

		strcat(data, "  HTTP / 1.1\n\n");

	}
	//if the request has PUT in it
	else if (strcmp(requestType, "PUT") == 0)
	{
		printf("Enter post number:");
		gets(choice);

		//creating request
		strcat(data, "PUT http://127.0.0.1:5000/posts/");

		strcat(data, choice);

		strcat(data, "  HTTP / 1.1\n\n");

		//get data from user
		printf("Enter Author:");
		gets(inputAuthor);

		printf("Enter Topic:");
		gets(inputTopic);

		printf("Enter Post:");
		gets(inputPost);
	
		strcat(data, inputAuthor);
		strcat(data, "_");

		strcat(data, inputTopic);
		strcat(data, "_");

		strcat(data, inputPost);
		strcat(data, "_");
	}
	else
	{
		return;
	}
}


//this funciton gets the 
SOCKET initializeClientSocket()
{
	//check version is correct
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	//check which version of winsock is being used
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	//initailizing socket
	SOCKET sock = 0;
	struct sockaddr_in address;

	//create socket with tcp
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Socket failed to create\n");
		return -1;
	}

	//initailize socket variables
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

	//use local host and tcp
	if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
	{
		printf("Invailid address\n");
		return -1;
	}

	//connection of socket, checking if failed
	if (connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		printf("Connection Failed \n");
		return -1;
	}
	return sock;
}

//this function will take a file and convert it into a sendable request
char* fileToPacket(char const* fileName,char* data) ///
{
	//open file
	int fileSize;
	FILE *file = fopen(fileName, "r"); ///
	//check if the file is valid
	if (file == NULL)
	{
		printf("Open file failed \n");
		return -1;
	}
	//find the size
	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	rewind(file);
	//read the file with the size
	fread(data, 1, fileSize, file);
	//close file and check if it worked properly.
	if (fclose(file) != 0)
	{
		printf("Close file failed \n");
		return -1;
	}

	return data; ///
}

int main()
{
	//initializing variables for later use.
	char buffer[8400] = { 0 };
	char data[1024] = { 0 };
	int valread;
	char input[2] = { 0 };

	//initialize the socket
	SOCKET sock = initializeClientSocket();
	int choice = 0;
	printf("Welcome to a RESTful API\n");
	do
	{
		//menu print
		printf("1. Get collection\n");
		printf("2. Get one\n");
		printf("3. Delete one\n");
		printf("4. Add one\n");
		printf("5. Update one\n");
		printf("0. Quit\n");
		gets(input);

		choice = atoi(input);

		//looping menu
		switch (choice)
		{
		//get the whole collection of articles
		case 1:
			//make a get all request packet
			fileToPacket("get-all.txt", data);
			
			//send the packet
			send(sock, data, strlen(data), 0);
			printf("Get all articles request sent.\n\n");
						
			//recieve all posts
			valread = recv(sock, buffer, 8400, 0);
			printf("%s\n", buffer);

			break;
		//get one article
		case 2:
			//making packet
			constructRequest("GET", data);

			//sending packet
			send(sock, data, strlen(data), 0);
			printf("Get article request sent.\n\n");

			//recieve stringified article packet.
			valread = recv(sock, buffer, 1024, 0);
			printf("Requested post:\n");
			printf("%s\n", buffer);
			break;
		//delete an article
		case 3:
			//making packet
			constructRequest("DELETE", data);

			//send packet
			send(sock, data, strlen(data), 0);
			printf("Delete post request sent\n\n");

			//recieve confirmation packet
			valread = recv(sock, buffer, 1024, 0);
			printf("%s\n", buffer);
			break;
		//add a new article
		case 4:
			//create a post packet
			constructRequest("POST",data);

			//send packet
			send(sock, data, strlen(data), 0);
			printf("Add new post request sent\n\n");
			
			//recieve packet
			valread = recv(sock, buffer, 1024, 0);
			printf("%s\n", buffer);
			break;
		//update an article
		case 5:
			constructRequest("PUT", data);

			//send packet
			send(sock, data, strlen(data), 0);
			printf("Update post request sent\n\n");

			//recieve packet
			valread = recv(sock, buffer, 1024, 0);
			printf("%s\n", buffer);
			break;
		//quit
		case 0:
			//send quit request
			send(sock, "Quit", strlen("Quit"), 0);
			printf("Quit request sent\n\n");

			printf("Goodbye.\n");
			break;
		default:
			//validation checking
			printf("Invaild Input, try again.\n\n");
		}
		//clearing buffer and data between requests to avoid stacking of requests.
		for (int i = 0; i < 1024; i++)
		{
			buffer[i] = 0;
			data[i] = 0;
		}
		//while loop needed for more then one request.
	} while (choice != 0);	
	
	return 0;

}
