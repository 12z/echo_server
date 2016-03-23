#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 9999
#define BUFFER_SIZE 255

int main(void) {
    fd_set rfds; // set of file descriptors to watch
    int retval; // return value by select function
    char buffer[BUFFER_SIZE + 1]; // buffer for messages
    ssize_t m_size; // message size
    int sockfd; // socket file descriptor
    struct sockaddr_in serv_addr; // socket addres of the server
    struct sockaddr_in client_addr; // socket address of the client from which message came
    socklen_t client_len; // length of client addr structure
    struct hostent *hostp; // hostname of a client from which message came
    char *hostaddrp; // IP address of a client

    client_len = sizeof(client_addr); // length of structure

    // opening UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    // filling addr structure and binding socket to it
    memset(&serv_addr, 0, sizeof(serv_addr)); // clear the structure
    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY; // receive from any address
    serv_addr.sin_port = htons(PORT); // port to listen
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    while (1) {
        FD_ZERO(&rfds); // clears the set of file descriptors to watch
        FD_SET(sockfd, &rfds); // add out socket to the list that should we watched

        retval = select(sockfd + 1, &rfds, NULL, NULL, NULL); // wait for data to arrive

        if (retval == -1) {
            perror("select()");
            exit(EXIT_FAILURE);
        } else if (!retval) {
            perror("No data in socket"); // should never happen as we will wait infinitely for data
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sockfd, &rfds)) { // check that select returned our socket
            memset(&buffer, 0, BUFFER_SIZE + 1); // clear the buffer
            m_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                              (struct sockaddr *) &client_addr, &client_len); // get the message
            if (m_size < 0) {
                perror("Error on read from socket");
                exit(EXIT_FAILURE);
            }
            hostp = gethostbyaddr((const char *) &client_addr.sin_addr.s_addr,
                                  sizeof(client_addr.sin_addr.s_addr), AF_INET); // get the address of a client
            if (hostp == NULL) {
                perror("ERROR on gethostbyaddr");
                exit(EXIT_FAILURE);
            }
            hostaddrp = inet_ntoa(client_addr.sin_addr); // get IP address of a client as a string
            if (hostaddrp == NULL) {
                perror("ERROR on inet_ntoa\n");
                exit(EXIT_FAILURE);
            }
            printf("server received datagram from %s (%s)\n",
                   hostp->h_name, hostaddrp); // print client data
            printf("server received %d/%d bytes: %s\n",
                   (int) strlen(buffer), (int) m_size, buffer); // print message


            m_size = sendto(sockfd, buffer, strlen(buffer), 0,
                            (struct sockaddr *) &client_addr, client_len); // send the same message back
            if (m_size < 0) {
                perror("ERROR in sendto");
                exit(EXIT_FAILURE);
            }
        }
    }
}