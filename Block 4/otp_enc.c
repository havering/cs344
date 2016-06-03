// Diana O'Haver
// Program 4
// File: otp_enc.c

// Program to ask otp_enc_d to perform encryption
// Program should:
//      - Check for key or plaintext files with bad characters
//      - Check if keyfile is shorter than plaintext
//      - Should NOT connect to otp_dec_d
//      - Output all errors to stderr

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>


// globals - largest file we're working with is < 70k characters
int MAX_SIZE = 70000;

/* Function to check that key and plaintext files contain valid characters */
void goodChars(char *filename) {
    FILE *fp;
    int c;

    fp = fopen(filename, "r");   // only need to open it read-only

    if (fp == NULL) {
        perror("file open error");
        exit(1);
    }

    // usage of fgetc: http://www.tutorialspoint.com/c_standard_library/c_function_fgetc.htm
    do {
       c = fgetc(fp);

       if (feof(fp)) {
            break;
       }

    // if it is between A and Z
    if (c < 65 || c > 90) {
        // if it isn't an all caps char, then it should only be a space or a newline
        if (c != 32 && c != 10) {
            printf("ERROR: invalid characters in %s\n", filename);
            exit(1);
        }
    }

    } while (1);

    fclose(fp);

    return;
}

/* Function to check the length of the file for comparison in main */
int findLength(char *filename) {
    FILE *fp;
    int c;
    int length = 0;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        perror("file open error");
        exit(1);
    }

    do {
        c = fgetc(fp);
        length++;

        if (feof(fp)) {
            break;
        }
    } while (1);

    fclose(fp);

    return length;
}

int main(int argc, char **argv) {
    char *keyname, *textname;
    int port, keyLength, plainLength, sockfd, sentBytes, fd, fdkey, fileSize, keySize, receivedBytes, i, fileResponse;
    struct sockaddr_in server;
    struct hostent *server_ip_address;       // this acts as client to otp_enc_d server
    char fileBuffer[MAX_SIZE];
    char keyBuffer[MAX_SIZE];
    char receiveBuffer[MAX_SIZE];
    char response[1];

    // zero out char arrays to ensure no garbage values
    memset(response, '0', 1);
    memset(fileBuffer, '0', MAX_SIZE);
    memset(keyBuffer, '0', MAX_SIZE);
    memset(receiveBuffer, '0', MAX_SIZE);

    /**** Input validation ****/
    // usage requires 4 arguments
    if (argc != 4) {
        printf("Usage: ./otp_enc plaintext key port\n");
        exit(1);
    }

    // grab the command line values
    keyname = argv[2];
    textname = argv[1];
    port = atoi(argv[3]);

    // error handling before we bother with anything else
    // first check that the key only has allowed characters
    // functions should error out if invalid characters are found, so no return value needed
    goodChars(keyname);

    // then check that the plaintext only has allowed characters
    goodChars(textname);

    // if it gets here without error, check the length of key and plaintext to ensure that key isn't shorter
    keyLength = findLength(keyname);

    plainLength = findLength(textname);

    if (keyLength < plainLength) {
        printf("ERROR: key must be longer than plaintext\n");
        exit(1);
    }

    // read plaintext file into a buffer to send to otp_enc_d
    if ((fd = open(textname, O_RDONLY)) == -1) {
        perror("file open error");
        exit(1);
    }

    fileSize = read(fd, fileBuffer, MAX_SIZE);

    // read key into a buffer to send to otp_enc_d
    if ((fdkey = open(keyname, O_RDONLY)) == -1) {
        perror("file open error");
        exit(1);
    }

    keySize = read(fdkey, keyBuffer, MAX_SIZE);

    // close them both
    close(fd);
    close(fdkey);

    /**** Set the socket and connection ****/
    // now that file validation is done, set up the connection
    // code borrowed and adapted from my own code in CS 372 (Networks) as well as Lecture 16
    // if otp_enc cannot find port given, should report error to screen with bad port and exit value of 2
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("otp_enc socket error on port %d\n", port);
        exit(2);
    }

    // make sure that server has no data in it
    memset(&server, '0', sizeof(server));

    server_ip_address = gethostbyname("localhost");

    if (server_ip_address == NULL) {
        fprintf(stderr, "could not resolve server host name\n");
        exit(1);
    }

    // from lectures - copy everything over from server_ip_address into server
    memcpy(&server.sin_addr, server_ip_address->h_addr, server_ip_address->h_length);

    // set the port to the given port and the version to IPv4
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("otp_enc connect error");
        exit(1);
    }

    /**** Send and receive data ****/

     // send the plaintext to otp_enc_d
    sentBytes = write(sockfd, fileBuffer, fileSize - 1);

    if (sentBytes < fileSize - 1) {
        perror("sent less than original file (file)");
        exit(1);
    }

    // get the response
    if ((fileResponse = read(sockfd, response, 1)) < 0) {
        perror("otp_enc response error");
        exit(1);
    }

    // now send the key
    sentBytes = write(sockfd, keyBuffer, keySize - 1);

    if (sentBytes < keySize - 1) {
        perror("sent less than original file (key)");
        exit(1);
    }

    // receive the response back from the server
    do {
        receivedBytes = read(sockfd, receiveBuffer, plainLength - 1);
    } while (receivedBytes > 0);

    if (receivedBytes == -1) {
        perror("error reading from otp_enc_d");
        exit(1);
    }

    // print it so the grading script can redirect it to the output file
    for (i = 0; i < plainLength - 1; i++) {
        printf("%c", receiveBuffer[i]);
    }

    // print newline to match specs
    printf("\n");

    // close it up
    close(sockfd);
    exit(0);
}
