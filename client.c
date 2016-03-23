#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 9999
#define BUFFER_SIZE 255

int main(int argc, char *argv[]) {
    int sockfd;
    ssize_t m_size;
    struct sockaddr_in server_addr;
    struct hostent *server;
    socklen_t server_len;
    char buffer[BUFFER_SIZE + 1];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *) &server_addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);
    server_addr.sin_port = htons(PORT);

    while (1) {

        /* get a message from the user */
        memset(buffer, 0, BUFFER_SIZE);
        printf("Please enter msg: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        /* send the message to the server */
        server_len = sizeof(server_addr);
        m_size = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *) &server_addr, server_len);
        if (m_size < 0) {
            perror("ERROR in sendto");
            exit(EXIT_FAILURE);
        }

        /* print the server's reply */
        m_size = recvfrom(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, &server_len);
        if (m_size < 0) {
            perror("ERROR in recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("Echo from server: %s", buffer);

    }
}