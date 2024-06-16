#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 1024

struct arg_struct {
    int arg1;
    int arg2;
};

void *readfile(void *arguments) {
    struct arg_struct *args = (struct arg_struct *)arguments;
    FILE *file;
    char buffer[MAX_LINE_LENGTH];
    size_t bytes_read;

    int i = args->arg1;
    int sd = args->arg2;

    char file_name[20];
    sprintf(file_name, "file%d.jpg", i + 1);
    file = fopen(file_name, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (write(sd, buffer, bytes_read) != bytes_read) {
            perror("Error sending file");
            break;
        }
    }

    if (feof(file)) {
        printf("File transfer completed\n");
    } else if (ferror(file)) {
        perror("Error reading file");
    }

    fclose(file);
    close(sd); // Close the socket after sending the file
    free(args); // Free the dynamically allocated memory
    return NULL;
}

int main(int argc, char **argv) {
    int sd, new_sd, port;
    struct sockaddr_in server_address, client_address;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Couldn't create socket\n");
        exit(1);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(port);

    if (bind(sd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Couldn't bind to socket\n");
        exit(2);
    }

    listen(sd, 5);
    pthread_t th[5];

    while (1) {
        socklen_t client_length = sizeof(client_address);
        for (int i = 0; i < 5; i++) {
            if ((new_sd = accept(sd, (struct sockaddr *)&client_address, &client_length)) == -1) {
                fprintf(stderr, "Couldn't connect to client\n");
                continue; // Continue to the next iteration
            }

            struct arg_struct *args = malloc(sizeof(struct arg_struct));
            if (args == NULL) {
                perror("Couldn't allocate memory for args");
                exit(1);
            }
            args->arg1 = i;
            args->arg2 = new_sd;

            if (pthread_create(&th[i], NULL, readfile, (void *)args) != 0) {
                perror("Error creating thread");
                close(new_sd);
                free(args);
            }
        }

        for (int i = 0; i < 5; i++) {
            pthread_join(th[i], NULL);
        }
    }

    close(sd);
    return 0;
}

