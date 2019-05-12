#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "dictionary.h"
#include "queue.h"

#define NUM_WORKERS 20

const char* DEFAULT_DIRECTORY = "words.txt";
const int DEFAULT_PORT = 9999;


Dictionary dict;
Queue* qlog;
Queue* qsocket;
pthread_t tid[NUM_WORKERS+1];
pthread_mutex_t lock_log, lock_socket;
pthread_cond_t cond_log, cond_socket;


char* toString(int);
void* worker();
void* logger();


//MAIN FUNCTION
int main(int argc, char *argv[]) {

    // arg1 is dict file, arg2 is port number - if user wishes

    if (argc >= 2) {

        dict = createDict(argv[1]);

    } else {

        dict = createDict(DEFAULT_DIRECTORY);
    }

    if (dict.size == 0) {
        printf("Error creating dictionary\n");
        return 1;
    }

    struct sockaddr_in server, client;
    int optval, c, socket_desc, new_socket;


    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Error creating socket\n");
        return 1;
    }

    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;


    if (argc == 3) {

        char *end;
        server.sin_port = htons((int)strtol(argv[2], &end, 10));

    } else {

        server.sin_port = htons(DEFAULT_PORT);
    }

    if (bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {

        printf("Bind error\n");
        return 1;
    }


    qlog = makeQueue();
    qsocket = makeQueue();


    if (pthread_mutex_init(&lock_log, NULL) != 0) {
        printf("Log mutex init error\n");
        exit (1);
    }

    if (pthread_mutex_init(&lock_socket, NULL) != 0) {
        printf("Socket mutex init error\n");
        exit (1);
    }


    for (int i = 0; i < NUM_WORKERS; i++) {

        int err = pthread_create(&(tid[i]), NULL, &worker, NULL);
        if (err != 0)
            printf("Error creating thread: %s", strerror(err));

    }

    int err = pthread_create(&(tid[NUM_WORKERS]), NULL, &logger, NULL);
    if (err != 0) {
        printf("Error creating logger thread: %s", strerror(err));
        exit(1);
    }

    listen(socket_desc, 3);

    printf("Spell Checker has been activated and is now accepting connections...\n");


    c = sizeof(struct sockaddr_in);

    while ((new_socket = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c))) {

        pthread_mutex_lock(&lock_socket);                                                       // Lock mutex

        push(qsocket, toString(new_socket));

        pthread_mutex_unlock(&lock_socket);                                                     // Unlock mutex
        pthread_cond_signal(&cond_socket);                                                      // Signal threads wake up
    }

}


char* toString(int val) {

    char* result = malloc(256 * sizeof(result));
    sprintf(result, "%d", val);
    return result;

}


void* worker() {
    while (1) {

        pthread_mutex_lock(&lock_socket);                                                         // Lock mutex
        pthread_cond_wait(&cond_socket, &lock_socket);

        if (qsocket->size < 1) {
            pthread_mutex_unlock(&lock_socket);
            continue;
        }

        char* end;
        int new_socket = strtol(pop(qsocket), &end, 10);
        pthread_mutex_unlock(&lock_socket);                                                       // Unlock mutex

        char buffer[256];
        int len;
        while (len = recv(new_socket, buffer, 256, 0) > 0) {

            buffer[strcspn(buffer, "\r\n")] = 0;
            toLower(buffer); // All lower for spell checking

            char message[256];
            strcpy(message, toString(new_socket));
            strcat(message, " - ");
            strcat(message, buffer);

            int found = spellCheck(dict, buffer);
            if (found) {
                strcat(message, " CORRECTLY SPELLED\n");
            } else {
                strcat(message, " MISSPELLED\n");
            }


            write(new_socket, message, strlen(message));

            pthread_mutex_lock(&lock_log);                                                          // Lock mutex

            push(qlog, message);

            pthread_mutex_unlock(&lock_log);                                                        // Unlock mutex
            pthread_cond_signal(&cond_log);                                                         // Signal threads wake up
        }
    }
}


void* logger() {

    while (1) {

        pthread_mutex_lock(&lock_log);                                                              // Lock mutex
        pthread_cond_wait(&cond_log, &lock_log);

        while (qlog->size > 0) {

            FILE* fp;
            fp = fopen("server.log", "a");

            char* message = pop(qlog);
            printf("%s", message);
            fputs(message, fp);

            fclose(fp);
        }
        pthread_mutex_unlock(&lock_log);                                                            // Unlock mutex
    }
}
