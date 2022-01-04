// Assignment 3 – Network Security – Winter 2021 - Milan Jovanovic
//
//
//  This file contains all rest methods for a server, as well as the server itself, with all of the saving and loading. The server contains articles.

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <ws2tcpip.h>
#include <stdbool.h>
#define PORT 5000
#define MAXLIST 15
#define MAXAUTHORLENG 15
#define MAXTOPICLENG 45
#define MAXPOSTLENG 500
#define MAXCAPACITY 600
#define MAXCAPACITYALL 8400
#define BUFFERSIZE 1024

//post from the python script is known here as article to eliminate confusion
//article structure made to hold the author, topic and post of the data.
struct Article
{
	char author[MAXAUTHORLENG];
	char topic[MAXTOPICLENG];
	char post[MAXPOSTLENG];
};

//articleArrLength is used to track the length of the array, this is used in functions to know exactly where it should end looking for articles.
int articleArrLength = 0;
struct Article articleArr[MAXLIST];

//used to delete articles
void deleteArticle(int index)
{
	//check if the index is vaild
	if (index < 0 || index > 14)
	{
		return;
	}
	//make an empty article to copy in the last spot
	struct Article empty = { NULL,NULL,NULL };

	//where you want to delete, make it equal to the next one, effectivly moving everything back one. 
	for (int i = index; i < 14;i++)
	{
		articleArr[i] = articleArr[i + 1];
	}
	//that leaves the last spot with a duplicate, so make it empty.
	articleArr[14] = empty;
	//subtract length as we just deleted an article
	articleArrLength--;
}

//used to make an article into a readable string form.
char* articleToString(int index)
{
	//check if index is valid
	if (index < 0 || index > 15)
	{
		return;
	}
	
	//take the specified article and put it in the buffer
	struct Article tempArticle = articleArr[index];
	char buffer[MAXCAPACITY] = { 0 };

	//concatonate the members of article to make a full article string
	strcat(buffer, tempArticle.author);
	strcat(buffer, "\n");
	strcat(buffer, tempArticle.topic);
	strcat(buffer, "\n");
	strcat(buffer, tempArticle.post);
	strcat(buffer, "\n");

	return buffer;
}

//used to update an existing article
void updateArticle(int index, char* data)
{
	//parce the data
	char* author = strtok(data, "_");
	char* topic = strtok(NULL, "_");
	char* post = strtok(NULL, "_");

	//take parced data and put it into temp variable
	struct Article tempArticle;
	strcpy(tempArticle.author, author);
	strcpy(tempArticle.topic, topic);
	strcpy(tempArticle.post, post);

	//set temp variable on article to be updated.
	articleArr[index] = tempArticle;
}

//used to save the artivles
void saveArticles()
{
	//open the file and loop throgh articleArrLength
	FILE* file;
	struct Article tempArticle;
	file = fopen("ArticleDatabase.txt", "w");
	for(int i = 0; i < articleArrLength; i++)
	{
		//make a temp article at index i
		struct Article tempArticle = articleArr[i];

		//check if save failed and put it into the file
		if (fputs(tempArticle.author,file) == EOF)
		{
			printf("save failed.");
			break;
		}
		//put new line character into file
		fputc('\n', file);

		//check if save failed and put it into the file
		if (fputs(tempArticle.topic, file) == EOF)
		{
			printf("save failed.");
			break;
		}
		//put new line character into file
		fputc('\n', file);

		//check if save failed and put it into the file
		if (fputs(tempArticle.post, file) == EOF)
		{
			printf("save failed.");
			break;
		}
		//put new line character into file
		fputs("\n\n", file);

	}
	fclose(file);
}

//load articles from text file database.
void loadArticles()
{
	//open file, make buffer for copying into 
	FILE* file;
	char buffer[MAXPOSTLENG] = { 0 };
	struct Article tempArticle; 
	file = fopen("ArticleDatabase.txt", "r");
	//counter for updating the article array
	int i = 0;
	while(1)
	{
		//if any of the gets are null, that means EOF was found, therefore break.
		//get the author and copy into the temp article
		if (fgets(buffer, MAXAUTHORLENG, file) == NULL)
		{
			break;
		}
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(tempArticle.author, buffer);
		
		//get the topic and copy into the temp article
		if (fgets(buffer, MAXTOPICLENG, file) == NULL)
		{
			break;
		}
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(tempArticle.topic, buffer);
	
		//get the post and copy into the temp article
		if (fgets(buffer, MAXPOSTLENG, file) == NULL)
		{
			break;
		}
		buffer[strlen(buffer) - 1] = '\0';
		strcpy(tempArticle.post, buffer);

		//set the temp article into the array and update the length of the array as we just added to it.
		articleArr[i] = tempArticle;
		i++;
		articleArrLength++;

		//skip the extra line that seperates posts.
		if (fgets(buffer, MAXPOSTLENG, file) == NULL)
		{
			break;
		}	
	}
	fclose(file);
}

//this will display all articles inside the array.
char* displayAllArticles()
{
	//a temp article is used to get and article, and is dumped into the buffer
	struct Article tempArticle;
	char buffer[1024] = { '\0' };

	//loop through all and get all articles that exist with the articleArrLength
	for (int i = 0; i < articleArrLength; i++)
	{
		tempArticle = articleArr[i];

		strcat(buffer, tempArticle.author);
		strcat(buffer, "\n");
	
		strcat(buffer, tempArticle.topic);
		strcat(buffer, "\n");
		
		strcat(buffer, tempArticle.post);
		strcat(buffer, "\n");
		strcat(buffer, "\n");
	}

	return buffer;
}

//add a new article to the list
void addNewArticle(char* data)
{
	//check if the list is full
	if (articleArrLength == MAXLIST)
	{
		printf("cannot add to list\n");
		return;
	}

	//parce the data
	char* author = strtok(data, "_");
	char* topic = strtok(NULL, "_");
	char* post = strtok(NULL, "_");

	//make a temp article to add to array
	struct Article tempArticle;
	
	//populate temp article
	strcpy(tempArticle.author, author);
	strcpy(tempArticle.topic, topic);
	strcpy(tempArticle.post, post);

	//temp article is added to array at the array length
	articleArr[articleArrLength] = tempArticle;

	//update array length because we just added
	articleArrLength++;
}

//this function is used for the packetization of display all articles and add new article 
char* articleList(char* requestType, char* data)
{
	//if the request type is get, and there is no specifed id, then it must be see all articles
	if (strcmp(requestType,"GET")== 0)
	{
		return displayAllArticles();
	}
	//if the request type is post, it must mean post an article
	else if (strcmp(requestType, "POST") == 0)
	{
		addNewArticle(data);
		return "Article added";
	}
	else
	{
		return "failed to get request type";
	}
}

//this function is used for the packetization of display a article,delete  article, and update article
char* individualArticle(char* requestType, char* data,int id)
{
	//if the request type is get, and there is an specifed id, then it must be see all articles
	if (strcmp(requestType, "GET") == 0)
	{
		return articleToString(id);
	}
	//if the request type is delete, it must mean delete an article
	else if (strcmp(requestType, "DELETE") == 0)
	{
		deleteArticle(id);
		return "Article deleted";
	}
	//if the request type is put, it must mean update an article
	else if (strcmp(requestType, "PUT") == 0)
	{
		updateArticle(id,data);
		return articleToString(id);
	}
	else
	{
		return "failed to get request type";
	}
}



int main()
{
	//load articles into array
	loadArticles();

	//check version of server
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

	//initailize socket and server peices
	SOCKET server;
	SOCKET newSocket;
	unsigned int valread;


	//setting up the sockaddr_in structure, named address, this defines that it is using TCP, the address of the address, and the port of 5000
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	int addresslength = sizeof(address);
	char buffer[BUFFERSIZE] = { 0 };

	//create socket 
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	//bind socket with self address and port 5000
	if (bind(server, (struct sockaddr*)&address,sizeof(address)) < 0)
	{
		printf("%d\n",WSAGetLastError());
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	//listen wait for connection
	if ((listen(server, 2)) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	
	//accept the packet
	if ((newSocket = accept(server, (struct sockaddr*)&address, (int*)&addresslength)) < 0)
	{
		perror("accept failed");
		exit(EXIT_FAILURE);
	}

	//while loop goes untill quit, to allow multiple actions
	while (1)
	{
		//get the packet and process data
		valread = recv(newSocket, buffer, BUFFERSIZE, 0);
		printf("%s\n", buffer);

		//if the buffer is quit, then save the articles and end the server.
		if (strcmp(buffer, "Quit") == 0)
		{
			saveArticles();
			break;
		}

		//parse packet
		char* data = (strstr(buffer, "\n\n") + 2);
		char* requestType = strtok(buffer, " ");
		char* url = strtok(NULL, " ");
		char* HTTPVersion = strtok(NULL, "\n");
		char* resource = strstr(url, "/posts");
		char* packet = "failed to compile packet";


		//get all articles, add new article.
		if (strcmp(resource, "/posts") == 0)
		{
			//get the packet from article listm as it is a function that requires no id
			packet = articleList(requestType, data);
			send(newSocket, packet, strlen(packet), 0);
		}
		// get single article, delete single article, update single article (needs id)
		else
		{
			int id = atoi(strrchr(resource, '/') + 1);
			packet = individualArticle(requestType, data, id);
			send(newSocket, packet, strlen(packet), 0);
		}
		//clean out the buffer
		for (int i = 0; i < 1024; i++)
		{
			buffer[i] = 0;
		}
	}
	return 0;

}




