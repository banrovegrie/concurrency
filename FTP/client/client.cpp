#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string.h>
#include <arpa/inet.h>

using namespace std;

#define BUFFER_SIZE 4096
#define ADDR "127.0.0.1"
#define PORT_NUM (int)8080

int init(int port)
{
	int sockfd;
	struct sockaddr_in addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cerr << "error: creating the socket" << endl;
		exit(2);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ADDR);
	addr.sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		cerr << "error: creating data channel" << endl;
		exit(3);
	}

	return (sockfd);
}

int main()
{
	struct sockaddr_in servaddr;
	char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];

	int sockfd = init(PORT_NUM);

	cout << "client>";

	while (fgets(sendline, BUFFER_SIZE, stdin) != NULL)
	{

		send(sockfd, sendline, BUFFER_SIZE, 0);
		char *token, *dummy;
		dummy = sendline;
		token = strtok(dummy, " ");

		if (strcmp("exit\n", sendline) == 0)
		{
			cerr << "Adiosss..." << endl;
			return 0;
		}
		else if (strcmp("get", token) == 0)
		{
			FILE *fp;
			int data_sockfd, sz;
			char port[BUFFER_SIZE], buffer[BUFFER_SIZE], block[BUFFER_SIZE], message[BUFFER_SIZE];
			
			recv(sockfd, port, BUFFER_SIZE, 0);
			data_sockfd = init(atoi(port));
			token = strtok(NULL, " \n");
			
			recv(sockfd, message, BUFFER_SIZE, 0);
			if (strcmp("1", message) == 0)
			{
				if ((fp = fopen(token, "w")) == NULL)
				{
					cout << "error: creating file\n";
				}
				else
				{
					recv(sockfd, block, BUFFER_SIZE, 0), sz = atoi(block);
					
					printf("Downloading File. Percentage --- 0");
					for (int i = 0; i < sz; i++)
					{
						recv(data_sockfd, buffer, BUFFER_SIZE, 0);
						fwrite(buffer, sizeof(char), BUFFER_SIZE, fp);
				
						printf("\rDownloading File. Percentage --- %f", (((float)i/sz)*100));
					}
					printf("\rDownloading File. Percentage -- 100.00\n");

					recv(sockfd, block, BUFFER_SIZE, 0), sz = atoi(block);

					if (sz > 0)
					{
						recv(data_sockfd, buffer, BUFFER_SIZE, 0);
						fwrite(buffer, sizeof(char), sz, fp);
					}
					fclose(fp);

					cout << "Downloaded File" << endl;
				}
			}
			else
			{
				cerr << "error: opening file" << endl;
			}
		}
		else
		{
			cerr << "error: in command" << endl;
		}

		cout << "client>";
	}

	cerr << "Adiosss..." << endl;

	exit(0);
}

