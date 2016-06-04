// Diana O'Haver
// Program 4
// File: otp_dec_d.c

// Program to decrypt ciphertext
// Program should:
//      - Return plaintext to otp_dec
//      - Run in the background as a daemon
//      - Listen on a port assigned when first ran
//      - Fork off process when making connection with otp_dec
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
        printf("Usage: ./otp_dec_d port\n");
        exit(1);
    }

    // grab command line argument into variable
    port = atoi(argv[1]);

    // now set up the socket connection for otp_enc to grab onto
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("otp_dec_d socket error on port %d\n", port);
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
            perror("otp_dec_d bind error");
            exit(1);
    }

    // then listen for connections, number limited by second argument
    if ((listen(sockfd, MAX_SOCKETS)) < 0) {
        perror("otp_dec_d listen error");
        exit(1);
    }

    len = sizeof(client);

    for(;;) {
        // connect to client
        if ((client_sockfd = accept(sockfd, (struct sockaddr *) &client, &len)) < 0) {
            perror("otp_dec_d accept error");
            // don't exit since program can be accepting of multiple connections
            continue;
        }
        // if no error, go ahead and fork
        //else {
            pid = fork();

            if (pid < 0) {
                perror("otp_dec_d fork error");
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
                    perror("otp_dec_d write error");
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
                    //printf("text is %c\n", textBuffer[i]);
                    //printf("key is %c\n", keyBuffer[i]);
                    // typecast the chars to ints since we're doing math
                    int text = (int) textBuffer[i];
                    int key = (int) keyBuffer[i];

                    //printf("received text %d\n", text);
                    //printf("received key %d\n", key);

                    // example given in specs has A - Z in the range of 0 - 26, so 27 with space
                    // subtract 64 to get values within that range
                    text = text - 65;
                    key = key - 65;

                    //printf("text after -65 is %d\n", text);
                    //printf("key after -65 is %d\n", key);

                    // subtract the text and key from each other
                    int temp = text - key;

                    //printf("temp is %d\n", temp);

                    if (temp < 27) {
                        temp = temp + 27;
                        //printf("temp was > 27, now is %d\n", temp);
                    }

                    temp = temp % 27;

                    //printf("temp mod 27 is now %d\n", temp);

                    // if it's 26, it's a space, and it should be converted back to a space
                    // which doesn't involve adding 65
                    if (temp == 26) {
                        temp = temp + 6;
                    }

                    else {
                        // bring it back into regular ASCII range
                        temp = temp + 65;
                    }


                    //printf("adding 65 to temp makes %d\n", temp);


                    temp = (char) temp;

                    sendBuffer[i] = temp;
                    //printf("sending temp %c\n", sendBuffer[i]);
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

                // need exit(0) or else this will keep trying the closed socket: http://stackoverflow.com/questions/13287359/socket-bad-file-descriptor
                exit(0);
            }
            else {
                // if it is parent, do close it up
                close(client_sockfd);
            }
        //}

    }

}
