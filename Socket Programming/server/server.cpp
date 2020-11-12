#include <bits/stdc++.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFERSIZE 8192 * sizeof(char)
#define endl '\n'
#define PRTCL AF_INET

using namespace std;

int init()
{
    int sockfd;
    if ((sockfd = socket(PRTCL, SOCK_STREAM, 0)) < 0)
        cerr << "error: setting socket" << endl;

    struct sockaddr_in addr;

    if (bind(sockfd, (sockaddr *)&addr, sizeof(addr)) < 0)
        cerr << "error: while binding" << endl;
    if (listen(sockfd, 1) < 0)
        cerr << "error: while listening" << endl;

    int new_sockfd;
    if ((new_sockfd = accept(sockfd, (sockaddr *)&addr, (socklen_t *)((int)sizeof(addr)))) < 0)
        cerr << "error: in accepting" << endl;

    return new_sockfd;
}

int download(char *str, int sockfd)
{
    int f, size;
    char command[5], filename[20];
    sscanf(str, "%s", command);

    struct stat obj;

    if (strcmp(command, "get") == 0)
    {
        sscanf(str, "%s %s", filename, filename);
        stat(filename, &obj);
        f = open(filename, O_RDONLY);
        size = obj.st_size;

        if (f == -1)
            size = 0;
        send(sockfd, &size, sizeof(int), 0);

        if (size)
            sendfile(sockfd, f, NULL, size);
    }
    else if (strcmp(command, "quit") == 0)
    {
        printf("Adiossss......\n");
        exit(0);
    }
    else
    {
        printf("It wasn't me - Shaggy");
    }

    return 0;
}

int main()
{
    int sockfd = init();

    while (true)
    {
        char *str = (char *)malloc(BUFFERSIZE);

        int val;
        if ((val = recv(sockfd, str, BUFFERSIZE, 0)) < 0)
        {
            cerr << "error: receiving" << endl;
            free(str);
            break;
        }
        else if (val == 0)
        {
            cerr << "Ending ..." << endl;
            free(str);
            break;
        }

        download(str, sockfd);
        free(str);
    }

    return 0;
}
