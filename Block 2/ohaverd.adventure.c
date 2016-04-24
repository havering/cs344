/* Diana O'Haver
   Program 2
   ohaverd.adventure.c */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Define strings - since they won't change, global should be fine */

char error[] = "HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.";
char curr[] = "CURRENT LOCATION: ";
char poss[] = "POSSIBLE CONNECTIONS: ";
char where[] = "WHERE TO? >";

/* Room names */
char orange[] = "Orange";
char red[] = "Red";
char yellow[] = "Yellow";
char green[] = "Green";
char blue[] = "Blue";
char purple[] = "Purple";
char pink[] = "Pink";
char black[] = "Black";
char brown[] = "Brown";
char silver[] = "Silver";

const int MAX_ROOMS = 7;

/* enum room types for use later */
enum room_type {
    START_ROOM,        // 0
    MID_ROOM,          // 1
    END_ROOM           // 2
};

/* Each room of type struct to hold connections and file values */
typedef struct room room;       // so i can just declare rooms without struct, i guess i might care here

struct room {
    int numConnections;
    int roomtype;
    room *connection1;
    room *connection2;
    room *connection3;
    room *connection4;
    room *connection5;
    room *connection6;
    char name[];                // must  be at end of struct because it is a "flexible array member"
};

/* Generate a random number between 3 and 6 to determine how many connections room will have */
int randNum() {
    return rand() % (6 + 1 - 3) + 3;
}

/* Generate random number between 0 and 9 to determine what the room's name is */
int randRoom() {
    return rand() % (9 + 0 - 0) + 0;
}
void generateRoomFiles() {
    int buffer = 30;
    int pid = getpid();
    char prefix[] = "ohaverd.rooms.";
    int i;
    int findName, numCons, totalRooms;

    char *dirName = malloc(buffer);

    // mash everything together into one string called dirName
    snprintf(dirName, buffer, "%s%d", prefix, pid);

    // example of creating a new directory if it doesn't already exist
    // sourced from: http://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
    struct stat st = {0};
    if (stat(dirName, &st) == -1) {         // if it is -1, then the directory doesn't already exist
        mkdir(dirName, 0755);              // 755 means that everyone can read and execute, but only owner can write
    }

    // put the room name options into an array of pointers to the actual static strings
    char *roomNames[10];
    roomNames[0] = red;
    roomNames[1] = orange;
    roomNames[2] = yellow;
    roomNames[3] = green;
    roomNames[4] = blue;
    roomNames[5] = purple;
    roomNames[6] = black;
    roomNames[7] = brown;
    roomNames[8] = silver;
    roomNames[9] = pink;

    // set up the START_ROOM first
    room one;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if ((i == findName)) {
            strcpy(one.name, roomNames[i]);
            // set that place to null so it won't be used again
            roomNames[i] = NULL;
        }
    }


    // set up the rest of the start room
    one.roomtype = START_ROOM;

    // find the number of connections it can have
    numCons = randNum();

    one.numConnections = numCons;

    // now it has a name, a room type, and a number of connections
    // the rest of the rooms should be created before connections are created
    // not DRY but refactor later maybe

    room two;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
            while (roomNames[i] == NULL) {
                if (i >= 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(two.name, roomNames[i]);
            // set that place to null so it won't be used again
            roomNames[i] = NULL;
            break;
        }
    }

    two.roomtype = MID_ROOM;

    numCons = randNum();

    two.numConnections = numCons;

    room three;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
            while (roomNames[i] == NULL) {
                if (i >= 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(three.name, roomNames[i]);
            // set that place to null so it won't be used again
            roomNames[i] = NULL;
            break;
        }
    }
    three.roomtype = MID_ROOM;

    numCons = randNum();

    three.numConnections = numCons;

    room four;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
            while (roomNames[i] == NULL) {
                if (i >= 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(four.name, roomNames[i]);
            // set that place to null so it won't be used again
            roomNames[i] = NULL;
            break;
        }
    }

    four.roomtype = MID_ROOM;

    numCons = randNum();

    four.numConnections = numCons;

    room five;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
            while (roomNames[i] == NULL) {
                if (i >= 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(five.name, roomNames[i]);
            // set that place to null so it won't be used again
            roomNames[i] = NULL;
            break;
        }
    }

    five.roomtype = MID_ROOM;

    numCons = randNum();

    five.numConnections = numCons;

    room six;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
            while (roomNames[i] == NULL) {
                if (i >= 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(six.name, roomNames[i]);
            // set that place to null so it won't be used again
            roomNames[i] = NULL;
            break;
        }
    }

    six.roomtype = MID_ROOM;

    numCons = randNum();

    six.numConnections = numCons;

    room seven;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
            while (roomNames[i] == NULL) {
                if (i >= 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(seven.name, roomNames[i]);
            // set that place to null so it won't be used again
            roomNames[i] = NULL;
            break;
        }
    }

    seven.roomtype = END_ROOM;

    numCons = randNum();

    seven.numConnections = numCons;


}

int main(void) {
    srand(time(NULL));      // seed rand
    // The first thing your program must do is generate 7 different room files, one room per file, in a directory called <username>.rooms.<process id>
    generateRoomFiles();


}
