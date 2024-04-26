#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int socketfd, clientfd, port;
    int yes = 1;
    char buffer[256];
    struct sockaddr_in server_addr, client_addr;
    port = atoi(argv[1]);

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Socket not created\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "Can't bind\n");
        exit(1);
    }

    listen(socketfd, 5);

    while (1) {
        int len = sizeof(client_addr);
        if ((clientfd = accept(socketfd, (struct sockaddr *)&client_addr, &len)) == -1) {
            fprintf(stderr, "Can't accept client\n");
            exit(1);
        }
        
        FILE *fp = fopen("img.jpg", "rb");
        if (fp == NULL) {
            fprintf(stderr, "Can't open file\n");
            exit(1);
        }

        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            if (write(clientfd, buffer, bytes_read) != bytes_read) {
                fprintf(stderr, "Error writing to client\n");
                exit(1);
            }
            printf("%s", buffer); // Assuming buffer contains text data
        }

        fclose(fp); // Close the file after reading
        close(clientfd);
    }

    close(socketfd);
    return 0;
}

