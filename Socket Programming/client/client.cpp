#include <bits/stdc++.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fstream>

#define PORT 8080
#define BUFFERSIZE 8192 * sizeof(char)
#define endl '\n'
#define PRTCL AF_INET

using namespace std;

int init()
{
    int sockfd;
    if ((sockfd = socket(PRTCL, SOCK_STREAM, 0)) < 0)
    {
        cerr << "error: setting socket" << endl;
        exit(0);
    }

    struct sockaddr_in addr;
    memset(&addr, '0', sizeof(addr));

    addr.sin_family = PRTCL;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        cerr << "error: while connecting from client" << endl;
        exit(0);
    }

    return sockfd;
}

int main()
{
    int sockfd = init();

    while (true)
    {
        char *str = (char *)malloc(BUFFERSIZE);

        if (read(0, str, BUFFERSIZE) <= 0)
        {
            cerr << "error: in reading from user" << endl;
            break;
        }

        if (send(sockfd, str, sizeof(str), 0) == -1)
        {
            cerr << "error: in sending from client";
            break;
        }
    }
    return 0;
}