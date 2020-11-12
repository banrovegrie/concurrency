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

int download(char *filename, char *str, char *f, int size)
{
    int fd;

    while (true)
    {
        fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
        if (fd == -1)
            cout << "F*** my life" << endl;
        else
            break;
    }

    write(fd, f, size);
    close(fd);

    strcpy(str, "cat ");
    strcat(str, filename);
    system(str);

    return 0;
}

int main()
{
    int sockfd = init();

    while (true)
    {
        printf(">");

        int size;
        char command[5], filename[20];
        scanf("%s", command);

        char *str = (char *)malloc(BUFFERSIZE);

        if (strcmp(command, "get"))
        {
            scanf("%s", filename);

            strcpy(str, "get ");
            strcpy(str, filename);

            send(sockfd, str, BUFFERSIZE, 0);
            recv(sockfd, &size, sizeof(int), 0);

            if (!size)
                printf("No such file on the remote directory\n");
            else
            {
                char *f = (char *)malloc(size);
                recv(sockfd, f, size, 0);
                download(filename, str, f, size);
            }
        }
        else if (strcmp(command, "quit"))
        {
            strcpy(str, "quit");
            send(sockfd, str, 100, 0);

            cout << "Closing connection..." << endl;
            exit(0);
        }
    }
    return 0;
}