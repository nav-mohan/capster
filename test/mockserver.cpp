#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <vector>
#include <iostream>

#define READ_BUFFER_SIZE 1024
void open_and_send(const char *filename, int clientfd);
int make_server_socket(const char *portNumber);
void sigint_handler(int signum);
void sigpipe_handler(int signum);

int sockfd;
std::vector<int> clients;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("USAGE:\t./mockserver <PORT> <FILE> <FILE>...");
        return EXIT_FAILURE;
    }

    const char *portNumber = argv[1];
    std::vector<const char *> filenames;
    for(int i = 2; i < argc; i++)
        filenames.push_back(argv[i]);
    // const char *filename1 = argv[2];
    // const char *filename2 = argv[3];
    sockfd = make_server_socket(portNumber);
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, sigpipe_handler);

    while (1)
    {
        printf("Listening...\n");
        // Start listening for incoming connections
        listen(sockfd, 5);
        int clientfd;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        printf("Accepting...\n");
        clients.push_back(clientfd);

        for(auto filename : filenames)
            open_and_send(filename,clientfd);

        // Close the client socket
        shutdown(clientfd,SHUT_RDWR);
        close(clientfd);
    }

    printf("EXITING NOW...\n");

    // Close the server socket
    close(sockfd);

    return 0;
}



int make_server_socket(const char *portNumber)
{
    // Create a socket
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
    int port = atoi(portNumber);
    // Bind the socket to a local address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    return sockfd;    
}


void open_and_send(const char *filename, int clientfd)
{
    int bytes_read,bytes_sent;
    char *buffer = (char*)malloc(READ_BUFFER_SIZE);
    FILE *xmlFile = fopen(filename,"r");
    while ((bytes_read = fread(buffer,sizeof(char),READ_BUFFER_SIZE,xmlFile)))
    {

        std::cout << "READ " << bytes_read << " BYTES" << std::endl;
        bytes_sent = send(clientfd,buffer,bytes_read,0);
        std::cout << "SENT " << bytes_sent << " BYTES" << std::endl;
        if(bytes_read != bytes_sent)
            return;
        memset(buffer,0,READ_BUFFER_SIZE);
    }
    printf("\nDone sending\n");
    fclose(xmlFile);
}


void sigint_handler(int signum) {
    printf("Caught SIGINT signal!\n");
   for (auto c : clients)
    {
        close(c);
        shutdown(c,SHUT_RDWR);
    }    
    close(sockfd);
    exit(1);
}

void sigpipe_handler(int signum) {
    printf("Caught SIGPIPE signal!\n");

    for (auto c : clients)
    {
        close(c);
        shutdown(c,SHUT_RDWR);
    }
}