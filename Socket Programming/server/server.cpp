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

int download(char *str)
{
    vector<char *> tokens;

    char *left, *token = strtok_r(str, " \t\n", &left);
    while (token != NULL)
        tokens.push_back(token), token = strtok_r(NULL, " \t\n", &left);

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

        download(str);
        free(str);
    }

    return 0;
}
