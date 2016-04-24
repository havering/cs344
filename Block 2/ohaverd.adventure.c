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

typedef int bool;
#define true 1
#define false 0

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
const int MIN_CONNS = 3;
const int MAX_CONNS = 6;

/* enum room types for use later */
enum room_type {
    START_ROOM,        // 0
    MID_ROOM,          // 1
    END_ROOM           // 2
};

/* Each room of type struct to hold connections and file values */
typedef struct room room;               // so i can just declare rooms without struct, i guess i might care here

struct room {
    int roomNum;
    int numConnections;
    int roomtype;
    room *connections[6];                // better - array of room connections so can be iterated through
    char name[10];                        // flexible array member is a terrible idea, use static value
};

/* Generate a random number between 3 and 6 to determine how many connections room will have */
int randNum() {
    return rand() % (6 + 1 - 3) + 3;
}

/* Generate random number between 0 and 9 to determine what the room's name is */
int randRoom() {
    return rand() % (9 + 0 - 0) + 0;
}

/* Generate random number between 1 and 7 to determine which room the room should be connected with */
int randConn() {
    return rand() % (7 + 1 - 1) + 1;
}

/* This tests if room1 is already connected to room2 */
bool isConnected(room room1, room room2) {
    int i;
    // if they are connected - shouldn't need this by the end of the very manual connection process
    if (room1.name == room2.name) {
        return true;
    }

    // loop through connections and make sure they aren't connected
    for (i = 0; i < room1.numConnections; i++) {
        if (room1.connections[i] == room2.connections[i]) {     // if they match
            if (room1.connections[i] != NULL) {                 // and they don't match because they're both null
                return true;
            }
        }
    }
    return false;               // if it makes it through the above and no match, return false
}

/* Connect two rooms together if possible */
void connectRooms(room *room1, room *room2) {
    int i;
    int connCount1 = 0;
    int connCount2 = 0;

    // first loop through and see how many connections have been made for both
    for (i = 0; i < room1->numConnections; i++) {
        if (room1->connections[i] != NULL) {
            connCount1++;
        }
    }

    printf("connCount1 = %d\n", connCount1);
    // if there is no room for more connections to room 1, return
    if (connCount1 >= room1->numConnections) {
        return;
    }

    for (i = 0; i < room2->numConnections; i++) {
        if (room2->connections[i] != NULL) {
            connCount2++;
        }
    }

    printf("connCount2 = %d\n", connCount2);
    // if there is no room for more connections to room 2, return
    if (connCount2 >= room2->numConnections) {
        return;
    }

    // if it gets here, then we should be able to connect the rooms
    // zero indexing allows inserting the connection at the connCount
    room1->connections[connCount1] = room2;
    room2->connections[connCount2] = room1;

    printf("room1 name is %s\n", room1->name);
    printf("room2 name is %s\n", room2->name);

    printf("room1 number is %d\n", room1->roomNum);
    printf("room2 number is %d\n", room2->roomNum);

    return;
}

/* Function to loop through and make sure everything is null */
void makeNull(room *inputRoom) {
    int i;

    for (i = 0; i < inputRoom->numConnections; i++) {
        inputRoom->connections[i] = NULL;
    }
}

void generateRoomFiles() {
    int buffer = 30;
    int pid = getpid();
    char prefix[] = "ohaverd.rooms.";
    int i;
    int findName;

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
    const char *roomNames[10];
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
            // set that place to taken so it won't be used again
            //strcpy(roomNames[i], "Taken");
            roomNames[i] = NULL;
        }
    }


    // set up the rest of the start room
    one.roomtype = START_ROOM;
    one.roomNum = 1;

    // find the number of connections it can have
    one.numConnections = randNum();

    // now it has a name, a room type, and a number of connections
    // the rest of the rooms should be created before connections are created
    // not DRY but refactor later maybe

    room two;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
           // while (strcmp(roomNames[i], "Taken") == 0) {
              while (roomNames[i] == NULL) {
                if (i == 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(two.name, roomNames[i]);
            // set that place to null so it won't be used again
            //strcpy(roomNames[i], "Taken");
            roomNames[i] = NULL;
            break;
        }
    }

    two.roomtype = MID_ROOM;
    two.roomNum = 2;

    // find the number of connections it can have
    two.numConnections = randNum();

    room three;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
           // while (strcmp(roomNames[i], "Taken") == 0) {
              while (roomNames[i] == NULL) {
                if (i == 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(three.name, roomNames[i]);
            // set that place to null so it won't be used again
            //strcpy(roomNames[i], "Taken");
            roomNames[i] = NULL;
            break;
        }
    }

    three.roomtype = MID_ROOM;
    three.roomNum = 3;

    // find the number of connections it can have
    three.numConnections = randNum();

    room four;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
           // while (strcmp(roomNames[i], "Taken") == 0) {
              while (roomNames[i] == NULL) {
                if (i == 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(four.name, roomNames[i]);
            // set that place to null so it won't be used again
            //strcpy(roomNames[i], "Taken");
            roomNames[i] = NULL;
            break;
        }
    }

    four.roomtype = MID_ROOM;
    four.roomNum = 4;

    // find the number of connections it can have
    four.numConnections = randNum();

    room five;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
           // while (strcmp(roomNames[i], "Taken") == 0) {
              while (roomNames[i] == NULL) {
                if (i == 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(five.name, roomNames[i]);
            // set that place to null so it won't be used again
            //strcpy(roomNames[i], "Taken");
            roomNames[i] = NULL;
            break;
        }
    }


    five.roomtype = MID_ROOM;
    five.roomNum = 5;

    // find the number of connections it can have
    five.numConnections = randNum();

    room six;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
           // while (strcmp(roomNames[i], "Taken") == 0) {
              while (roomNames[i] == NULL) {
                if (i == 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(six.name, roomNames[i]);
            // set that place to null so it won't be used again
            //strcpy(roomNames[i], "Taken");
            roomNames[i] = NULL;
            break;
        }
    }

    six.roomtype = MID_ROOM;
    six.roomNum = 6;

    // find the number of connections it can have
    six.numConnections = randNum();

    room seven;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
           // while (strcmp(roomNames[i], "Taken") == 0) {
              while (roomNames[i] == NULL) {
                if (i == 10) {
                    i = 0;
                }
                else {
                    i++;
                }
            }
            strcpy(seven.name, roomNames[i]);
            // set that place to null so it won't be used again
            //strcpy(roomNames[i], "Taken");
            roomNames[i] = NULL;
            break;
        }
    }


    seven.roomtype = END_ROOM;
    seven.roomNum = 7;

    // find the number of connections it can have
    seven.numConnections = randNum();

    printf("One has %d connections\n", one.numConnections);
    printf("Two has %d connections\n", two.numConnections);
    printf("Three has %d connections\n", three.numConnections);
    printf("Four has %d connections\n", four.numConnections);
    printf("Five has %d connections\n", five.numConnections);
    printf("Six has %d connections\n", six.numConnections);
    printf("Seven has %d connections\n", seven.numConnections);

//    printf("One's name is %s\n", one.name);
//    printf("Two's name is %s\n", two.name);
//    printf("Three's name is %s\n", three.name);
//    printf("Four's name is %s\n", four.name);
//    printf("Five's name is %s\n", five.name);
//    printf("Six's name is %s\n", six.name);
//    printf("Seven's name is %s\n", seven.name);

    // now there are seven rooms named one through seven
    // set up the connections

    int findConn;


    // loop through connections and make sure that they're null for everyone before we start making connections

    makeNull(&one);
    makeNull(&two);
    makeNull(&three);
    makeNull(&four);
    makeNull(&five);
    makeNull(&six);
    makeNull(&seven);

    for (i = 0; i < one.numConnections; i++) {
        //room *connectedRoom;

        while (one.connections[i] == NULL) {
            findConn = randConn();

            if (findConn == 2) {
                if (isConnected(one, two) == false) {
                    connectRooms(&one, &two);
                }
            }

            if (findConn == 3) {
                if (isConnected(one, three) == false) {
                    connectRooms(&one, &three);
                }
            }

            if (findConn == 4) {
                if (isConnected(one, four) == false) {
                    connectRooms(&one, &four);
                }
            }

            if (findConn == 5) {
                if (isConnected(one, five) == false) {
                    connectRooms(&one, &five);
                }
            }

            if (findConn == 6) {
                if (isConnected(one, six) == false) {
                    connectRooms(&one, &six);
                }
            }

            if (findConn == 7) {
                if (isConnected(one, seven) == false) {
                    connectRooms(&one, &seven);
                }
            }
        }


    }



    // at the end of this, one should have between 3 and 6 connections
    printf("one connections are:\n");

    for (i = 0; i < one.numConnections; i++) {
        if (one.connections[i] == NULL) {
            printf("Null value where you shouldn't have one\n");
        }
        else {
            printf("%s\n", one.connections[i]->name);
        }
    }
}

int main(void) {
    srand(time(NULL));      // seed rand
    // The first thing your program must do is generate 7 different room files, one room per file, in a directory called <username>.rooms.<process id>
    generateRoomFiles();

    return 0;
}
