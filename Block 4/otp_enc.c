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
#include <unistd.h>
#include <ctype.h>

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

    return length;
}

int main(int argc, char **argv) {
    char *keyname, *textname;
    int port, keyLength, plainLength;

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

    exit(0);
}
