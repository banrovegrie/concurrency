#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <unistd.h>

using namespace std;

#define BUFFER_SIZE 4096
#define ADDR "127.0.0.1"
#define PORT_NUM (int)8080

int init(int port)
{
	int listenfd;
	struct sockaddr_in addr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cerr << "error: creating data socket" << endl;
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	bind(listenfd, (struct sockaddr *)&addr, sizeof(addr));
	listen(listenfd, 1);

	return (listenfd);
}

int accepting(int sockfd)
{
	int dataconnfd;
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	if ((dataconnfd = accept(sockfd, (struct sockaddr *)&addr, &len)) < 0)
	{
		cerr << "error: accepting the data socket" << endl;
		exit(3);
	}

	return (dataconnfd);
}

int main()
{
	int listenfd = init(PORT_NUM);
	cout << "Server is up and waiting for connections" << endl;

	while(true)
	{
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		
		int connfd = accept(listenfd, (struct sockaddr *)&addr, &len);
		cout << "Received connection request" << endl;
		close(listenfd);

		int data_port = 1024, n;
		char buf[BUFFER_SIZE];

		while ((n = recv(connfd, buf, BUFFER_SIZE, 0)) > 0)
		{
			cout << "Command from client: " << buf;

			char *token, *dummy;
			dummy = buf;
			token = strtok(dummy, " ");

			if (strcmp("exit\n", buf) == 0)
			{
				cout << "Client Exited" << endl;
			}
			else if (strcmp("get", token) == 0)
			{
				FILE *fp;
				int data_sockfd, lSize, sz, sz_last;
				char port[BUFFER_SIZE], buffer[BUFFER_SIZE], block[BUFFER_SIZE];
				
				token = strtok(NULL, " \n");
				cout << "File asked by client: " << token << endl;
				
				if ((++data_port) == PORT_NUM) data_port++;
				
				sprintf(port, "%d", data_port);
				data_sockfd = init(data_port); 

				send(connfd, port, BUFFER_SIZE, 0);	
				data_sockfd = accepting(data_sockfd);	
				
				if ((fp = fopen(token, "r")) != NULL)
				{
					send(connfd, "1", BUFFER_SIZE, 0);
					fseek(fp, 0, SEEK_END);
					lSize = ftell(fp);
					rewind(fp);

					sz = lSize / BUFFER_SIZE, sz_last = lSize % BUFFER_SIZE;
					
					sprintf(block, "%d", sz);
					send(connfd, block, BUFFER_SIZE, 0);

					for (int i = 0; i < sz; i++)
					{
						fread(buffer, sizeof(char), BUFFER_SIZE, fp);
						send(data_sockfd, buffer, BUFFER_SIZE, 0);
					}

					sprintf(block, "%d", sz_last);
					send(connfd, block, BUFFER_SIZE, 0);
					
					if (sz_last > 0)
					{
						fread(buffer, sizeof(char), sz_last, fp);
						send(data_sockfd, buffer, BUFFER_SIZE, 0);
					}

					fclose(fp);
					cout << "Uploaded File" << endl;
				}
				else
				{
					send(connfd, "0", BUFFER_SIZE, 0);
				}
			}
		}

		if (n < 0)
		{
			cout << "error: in reading" << endl;
		}

		close(connfd);
	}
}
