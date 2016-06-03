// Diana O'Haver
// Program 4
// File: otp_enc_d.c

// Program to perform encoding
// Program should:
//      - Run in the background as a daemon
//      - Listen on a port assigned when first ran
//      - Fork off process when making connection with otp_enc
//      - Support up to five concurrent socket connections
//      - Not crash or exit upon errors
//      - Use localhost as target IP address/host

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>

// globals
int MAX_SOCKETS = 5;        // support up to 5 concurrent socket connections
int MAX_SIZE = 70000;       // largest file we're working with is < 70k characters


// code in part borrowed from lecture 16 and 17, and my own work in CS 372 (Networks)
// additional example of running process as a daemon from: http://www.thegeekstuff.com/2012/02/c-daemon-process/
int main(int argc, char **argv) {
    struct sockaddr_in server, client;
    int sockfd, port, client_sockfd, sentBytes, plainLength, keyLength, i;
    pid_t pid;
    socklen_t len;
    int one = 1;
    char textBuffer[MAX_SIZE];
    char keyBuffer[MAX_SIZE];
    char sendBuffer[MAX_SIZE];

    // housekeeping to ensure that the program is used correctly
    if (argc != 2) {
        printf("Usage: ./otp_enc_d port\n");
        exit(1);
    }

    // grab command line argument into variable
    port = atoi(argv[1]);

    // now set up the socket connection for otp_enc to grab onto
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("otp_enc_d socket error on port %d\n", port);
        exit(2);
    }

    // make sure that server has no data in it
    memset(&server, 0, sizeof(server));

    // set the server to ipv4, listening for all connections, using passed in port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // make it so that sockets can be reused
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
        perror("setsockopt error");
        exit(1);
    }

    // bind server to assigned port
     if ((bind(sockfd, (struct sockaddr*)&server, sizeof(server))) < 0) {
            perror("otp_enc_d bind error");
            exit(1);
    }

    // then listen for connections, number limited by second argument
    if ((listen(sockfd, MAX_SOCKETS)) < 0) {
        perror("otp_enc_d listen error");
        exit(1);
    }

    len = sizeof(client);

    for(;;) {
        // connect to client
        if ((client_sockfd = accept(sockfd, (struct sockaddr *) &client, &len)) < 0) {
            perror("otp_enc_d accept error");
            // don't exit since program can be accepting of multiple connections
            continue;
        }
        // if no error, go ahead and fork
        //else {
            pid = fork();

            if (pid < 0) {
                perror("otp_enc_d fork error");
            }

            // child process should be the one handling everything
            if (pid == 0) {
                // zero out buffers to ensure no garbage data
                memset(textBuffer, 0, MAX_SIZE);
                memset(keyBuffer, 0, MAX_SIZE);
                memset(sendBuffer, 0, MAX_SIZE);

                // plaintext is always sent first, grab that info
                if ((plainLength = read(client_sockfd, textBuffer, MAX_SIZE)) < 0) {
                    perror("error reading plaintext");
                    exit(1);
                }

                // send back single byte ACK
                sentBytes = write(client_sockfd, "k", 1);

                if (sentBytes < 0) {
                    perror("otp_enc_d write error");
                    exit(1);
                }

               // now get the key
                if ((keyLength = read(client_sockfd, keyBuffer, MAX_SIZE)) < 0) {
                    perror("error reading key");
                    exit(1);
                }
                // now encrypt the message using the key
                // loop through the plaintext - if key is longer than plaintext, nbd
                for (i = 0; i < plainLength; i++) {
                    // typecast the chars to ints since we're doing math
                    int text = (int) textBuffer[i];
                    int key = (int) keyBuffer[i];

                    // add 59 to space to bring it to the next value after Z
                    if (text == 32) {
                        text += 59;
                    }

                    if (key == 32) {
                        key += 59;
                    }

                    // example given in specs has A - Z in the range of 0 - 26, so 27 with space
                    // subtract 64 to get values within that range
                    text = text - 64;
                    key = key - 64;

                    // add the text and key back together
                    int temp = text + key;

                    if (temp <= 27) {
                        temp = temp % 27;
                    }
                    else {
                        temp = temp - 27;
                        temp = temp % 27;
                    }

                    // add 0 to convert back to char: http://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
                    sendBuffer[i] = temp + '0';
                }

                // now send the encrypted buffer back to the client
                sentBytes = write(client_sockfd, sendBuffer, plainLength);

                if (sentBytes < plainLength) {
                    printf("sent less than file back to client\n");
                    exit(1);
                }

                // close the sockets up
                close(client_sockfd);
                close(sockfd);
                exit(0);
            }
            else {
                // if it is parent, do close it up
                close(client_sockfd);
            }
        //}

    }

}
