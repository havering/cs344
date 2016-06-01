// Diana O'Haver
// Program 4
// File: keygen.c

// Program to create a key file
// Program should:
//      - Create key file of specified length
//      - Characters in file will be any of 27 allowed characters(A-Z and space)
//      - Randomly generate characters
//      - No created spaces every 5 characters
//      - Last character output is newline
//      - Send all error output to stderr

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// global array of allowed characters
char characters[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' ' };

// function to return a random number between 0 and 27
int randNum() {
    return rand() % 28;
}

// function to randomize allowed characters and write them to file of specified length
void randomize(int numChars) {
    //FILE *fp;
    int counter, num;

    //fp = fopen(filename, "w+");

    counter = 0;

    while (counter != numChars) {
        num = randNum();

        printf("%c", characters[num]);
        counter++;
    }

    // put a newline at the end per specs
    printf("\n");

    return;
}


int main(int argc, char **argv) {
    srand(time(NULL));
    // ensure enough arguments passed in
//    if (argc != 4) {
//        printf("Usage: keygen numchars > filename\n");
//        exit(1);
//    }

    //char *filename = argv[3];
    int numChars = atoi(argv[1]);

    //printf("numchars is %d", numChars);
    randomize(numChars);
}
