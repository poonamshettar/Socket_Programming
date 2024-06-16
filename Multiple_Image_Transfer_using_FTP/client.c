#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

void receive_file(int sd, const char *file_name) {
    FILE *file = fopen(file_name, "wb");
    if (file == NULL) {
        fprintf(stderr, "File couldn't be created\n");
        return;
    }

    char buffer[MAX_LINE_LENGTH];
    ssize_t bytes_read;

    while ((bytes_read = read(sd, buffer, sizeof(buffer))) > 0) {
        if (bytes_read < 0) {
            perror("Error reading from socket");
            break;
        }

        size_t bytes_written = fwrite(buffer, 1, bytes_read, file);
        if (bytes_written != bytes_read) {
            perror("Error writing to file");
            break;
        }
    }

    fclose(file);
}

int main(int argc, char **argv) {
    int sd, port;
    struct sockaddr_in server_address;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip address> <port>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);

    for (int i = 0; i < 5; i++) {
        if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            fprintf(stderr, "Couldn't create socket\n");
            exit(1);
        }
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(port);

        if (inet_aton(argv[1], &server_address.sin_addr) == 0) {
            fprintf(stderr, "Invalid address\n");
            close(sd);
            exit(1);
        }

        if (connect(sd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
            perror("Couldn't connect");
            close(sd);
            exit(1);
        }

        char file_name[20];
        sprintf(file_name, "opfile%d.jpg", i + 1);
        receive_file(sd, file_name);

        close(sd); // Close the socket after receiving the file
    }

    return 0;
}

