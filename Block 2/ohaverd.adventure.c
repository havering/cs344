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
    int tally;                          // tracking current number of connections
    int numConnections;                 // tracking total allowed connections
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
/* Return 0 for true, 1 for false */
int isConnected(room room1, room room2) {
    int i;
    // if they are connected - shouldn't need this by the end of the very manual connection process
    if (room1.name == room2.name) {
        return 0;
    }

    // loop through connections and make sure they aren't connected
    for (i = 0; i < room1.numConnections; i++) {
        if (room1.connections[i] != NULL) {                                 // don't bother checking null connections
           // printf("conns[i].name is %d\n", room1.connections[i]->roomNum);

            if (room1.connections[i]->roomNum == room2.roomNum) {                    // if they match
                //printf("Returning 0 because %s is connected to %s\n", room1.name, room2.name);
                return 0;
            }
        }
    }
    return 1;               // if it makes it through the above and no match, return false
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

    //printf("connCount1 = %d\n", connCount1);
    // if there is no room for more connections to room 1, return
    if (connCount1 >= room1->numConnections) {
        return;
    }

    for (i = 0; i < room2->numConnections; i++) {
        if (room2->connections[i] != NULL) {
            connCount2++;
        }
    }

    //printf("connCount2 = %d\n", connCount2);
    // if there is no room for more connections to room 2, return
    if (connCount2 >= room2->numConnections) {
        return;
    }

    // if it gets here, then we should be able to connect the rooms
    // zero indexing allows inserting the connection at the connCount
    room1->connections[connCount1] = room2;
    room2->connections[connCount2] = room1;

    room1->tally = room1->tally + 1;
    room2->tally = room2->tally + 1;

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
    one.tally = 0;

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
    two.tally = 0;

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
    three.tally = 0;

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
    four.tally = 0;

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
    five.tally = 0;

    // find the number of connections it can have
    five.numConnections = randNum();

    room six;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
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
            roomNames[i] = NULL;
            break;
        }
    }

    six.roomtype = MID_ROOM;
    six.roomNum = 6;
    six.tally = 0;

    // find the number of connections it can have
    six.numConnections = randNum();

    room seven;

    findName = randRoom();

    for (i = 0; i < 10; i++) {
        if (i == findName) {
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
            roomNames[i] = NULL;
            break;
        }
    }


    seven.roomtype = END_ROOM;
    seven.roomNum = 7;
    seven.tally = 0;

    // find the number of connections it can have
    seven.numConnections = randNum();

    printf("One (%s) has %d connections\n", one.name, one.numConnections);
    printf("Two (%s) has %d connections\n", two.name, two.numConnections);
    printf("Three (%s) has %d connections\n", three.name, three.numConnections);
    printf("Four (%s) has %d connections\n", four.name, four.numConnections);
    printf("Five (%s) has %d connections\n", five.name, five.numConnections);
    printf("Six (%s) has %d connections\n", six.name, six.numConnections);
    printf("Seven (%s) has %d connections\n", seven.name, seven.numConnections);

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


    /** Hook up room one **/
    for (i = 0; i < one.numConnections; i++) {

        while (one.connections[i] == NULL) {
            findConn = randConn();

            if (findConn == 2) {
                if (two.tally < two.numConnections) {
                    if (isConnected(one, two) == 1) {
                        connectRooms(&one, &two);
                    }
                }
            }

            if (findConn == 3) {
                if (three.tally < three.numConnections) {
                    if (isConnected(one, three) == 1) {
                        connectRooms(&one, &three);
                    }
                }
            }

            if (findConn == 4) {
                if (four.tally < four.numConnections) {
                    if (isConnected(one, four) == 1) {
                        connectRooms(&one, &four);
                    }
                }
            }

            if (findConn == 5) {
                 if (five.tally < five.numConnections) {
                    if (isConnected(one, five) == 1) {
                        connectRooms(&one, &five);
                    }
                }
            }

            if (findConn == 6) {
                if (six.tally < six.numConnections) {
                    if (isConnected(one, six) == 1) {
                        connectRooms(&one, &six);
                    }
                }
            }

            if (findConn == 7) {
                if (seven.tally < seven.numConnections) {
                    if (isConnected(one, seven) == 1) {
                        connectRooms(&one, &seven);
                    }
                }
            }
        }


    }



    // at the end of this, one should have between 3 and 6 connections
    printf("%s (1) connections are:\n", one.name);

    for (i = 0; i < one.numConnections; i++) {
        if (one.connections[i] == NULL) {
            printf("Null value where you shouldn't have one\n");
        }
        else {
            printf("%s\n", one.connections[i]->name);
        }
    }

    /** Hook up room two **/
    for (i = 0; i < two.numConnections; i++) {

        while (two.connections[i] == NULL) {
            findConn = randConn();

            if (findConn == 1) {
                if (one.tally < one.numConnections) {
                    if (isConnected(two, one) == 1) {
                        connectRooms(&two, &one);
                    }
                }
            }

            if (findConn == 3) {
                if (three.tally < three.numConnections) {
                if (isConnected(two, three) == 1) {
                    connectRooms(&two, &three);
                }
            }
            }

            if (findConn == 4) {
                if (four.tally < four.numConnections) {
                    if (isConnected(two, four) == 1) {
                        connectRooms(&two, &four);
                    }
                }
            }

            if (findConn == 5) {
                if (five.tally < five.numConnections) {
                if (isConnected(two, five) == 1) {
                    connectRooms(&two, &five);
                }
                }
            }

            if (findConn == 6) {
                if (six.tally < six.numConnections) {
                if (isConnected(two, six) == 1) {
                    connectRooms(&two, &six);
                }
                }
            }

            if (findConn == 7) {
                if (seven.tally < seven.numConnections) {
                if (isConnected(two, seven) == 1) {
                    connectRooms(&two, &seven);
                }
                }
            }
        }
    }

    printf("%s (2) connections are:\n", two.name);

    for (i = 0; i < two.numConnections; i++) {
        if (two.connections[i] == NULL) {
            printf("Null value where you shouldn't have one\n");
        }
        else {
            printf("%s\n", two.connections[i]->name);
        }
    }

    /** Hook up room three **/
    for (i = 0; i < three.numConnections; i++) {

        while (three.connections[i] == NULL) {
            findConn = randConn();

            if (findConn == 2) {
                if (two.tally < two.numConnections) {
                if (isConnected(three, two) == 1) {
                    connectRooms(&three, &two);
                }
                }
            }

            if (findConn == 1) {
                    if (one.tally < one.numConnections) {
                if (isConnected(three, one) == 1) {
                    connectRooms(&three, &one);
                }
                    }
            }

            if (findConn == 4) {
                    if (four.tally < four.numConnections) {
                if (isConnected(three, four) == 1) {
                    connectRooms(&three, &four);
                }
                    }
            }

            if (findConn == 5) {
                    if (five.tally < five.numConnections) {
                if (isConnected(three, five) == 1) {
                    connectRooms(&three, &five);
                }
                    }
            }

            if (findConn == 6) {
                    if (six.tally < six.numConnections) {
                if (isConnected(three, six) == 1) {
                    connectRooms(&three, &six);
                }
                    }
            }

            if (findConn == 7) {
                    if (seven.tally < seven.numConnections) {
                if (isConnected(three, seven) == 1) {
                    connectRooms(&three, &seven);
                }
                    }
            }
        }
    }

    printf("%s (3) connections are:\n", three.name);

    for (i = 0; i < three.numConnections; i++) {
        if (three.connections[i] == NULL) {
            printf("Null value where you shouldn't have one\n");
        }
        else {
            printf("%s\n", three.connections[i]->name);
        }
    }

    /** Hook up room four **/
    for (i = 0; i < four.numConnections; i++) {

        while (four.connections[i] == NULL) {
            findConn = randConn();

            if (findConn == 2) {
                    if (two.tally < two.numConnections) {
                if (isConnected(four, two) == 1) {
                    connectRooms(&four, &two);
                }
            }
            }

            if (findConn == 3) {
                    if (three.tally < three.numConnections) {
                if (isConnected(four, three) == 1) {
                    connectRooms(&four, &three);
                }
            }
            }

            if (findConn == 1) {
                    if (one.tally < one.numConnections) {
                if (isConnected(four, one) == 1) {
                    connectRooms(&four, &one);
                }
                    }
            }

            if (findConn == 5) {
                    if (five.tally < five.numConnections) {
                if (isConnected(four, five) == 1) {
                    connectRooms(&four, &five);
                }
                    }
            }

            if (findConn == 6) {
                    if (six.tally < six.numConnections) {
                if (isConnected(four, six) == 1) {
                    connectRooms(&four, &six);
                }
                    }
            }

            if (findConn == 7) {
                    if (seven.tally < seven.numConnections) {
                if (isConnected(four, seven) == 1) {
                    connectRooms(&four, &seven);
                }
            }
            }
        }
    }

    printf("%s (4) connections are:\n", four.name);

    for (i = 0; i < four.numConnections; i++) {
        if (four.connections[i] == NULL) {
            printf("Null value where you shouldn't have one\n");
        }
        else {
            printf("%s\n", four.connections[i]->name);
        }
    }

    /** Hook up room five **/
    for (i = 0; i < five.numConnections; i++) {

        while (five.connections[i] == NULL) {
            findConn = randConn();

            if (findConn == 2) {
                    if (two.tally < two.numConnections) {
                if (isConnected(five, two) == 1) {
                    connectRooms(&five, &two);
                }
                    }
            }

            if (findConn == 3) {
                    if (three.tally < three.numConnections) {
                if (isConnected(five, three) == 1) {
                    connectRooms(&five, &three);
                }
            }
            }

            if (findConn == 4) {
                    if (four.tally < four.numConnections) {
                if (isConnected(five, four) == 1) {
                    connectRooms(&five, &four);
                }
            }
            }

            if (findConn == 1) {
                    if (one.tally < one.numConnections) {
                if (isConnected(five, one) == 1) {
                    connectRooms(&five, &one);
                }
                    }
            }

            if (findConn == 6) {
                    if (six.tally < six.numConnections) {
                if (isConnected(five, six) == 1) {
                    connectRooms(&five, &six);
                }
                    }
            }

            if (findConn == 7) {
                    if (seven.tally < seven.numConnections) {
                if (isConnected(five, seven) == 1) {
                    connectRooms(&five, &seven);
                }
                }
            }
        }
    }

    printf("%s (5) connections are:\n", five.name);

    for (i = 0; i < five.numConnections; i++) {
        if (five.connections[i] == NULL) {
            printf("Null value where you shouldn't have one\n");
        }
        else {
            printf("%s\n", five.connections[i]->name);
        }
    }

    /** Hook up room six **/
    for (i = 0; i < six.numConnections; i++) {

        while (six.connections[i] == NULL) {
            findConn = randConn();

            if (findConn == 2) {
                    if (two.tally < two.numConnections) {
                if (isConnected(six, two) == 1) {
                    connectRooms(&six, &two);
                }
                    }
            }

            if (findConn == 3) {
                    if (three.tally < three.numConnections) {
                if (isConnected(six, three) == 1) {
                    connectRooms(&six, &three);
                }
            }
            }

            if (findConn == 4) {
                    if (four.tally < four.numConnections) {
                if (isConnected(six, four) == 1) {
                    connectRooms(&six, &four);
                }
            }
            }

            if (findConn == 5) {
                    if (five.tally < five.numConnections) {
                if (isConnected(six, five) == 1) {
                    connectRooms(&six, &five);
                }
            }
            }

            if (findConn == 1) {
                    if (one.tally < one.numConnections) {
                if (isConnected(six, one) == 1) {
                    connectRooms(&six, &one);
                }
            }
            }

            if (findConn == 7) {
                    if (seven.tally < seven.numConnections) {
                if (isConnected(six, seven) == 1) {
                    connectRooms(&six, &seven);
                }
                    }
            }
        }
    }

    printf("%s (6) connections are:\n", six.name);

    for (i = 0; i < six.numConnections; i++) {
        if (six.connections[i] == NULL) {
            printf("Null value where you shouldn't have one\n");
        }
        else {
            printf("%s\n", six.connections[i]->name);
        }
    }

    /** Hook up room seven **/
    for (i = 0; i < seven.numConnections; i++) {

        while (seven.connections[i] == NULL) {
            findConn = randConn();

            if (findConn == 2) {
                    if (two.tally < two.numConnections) {
                if (isConnected(seven, two) == 1) {
                    connectRooms(&seven, &two);
                }
                    }
            }

            if (findConn == 3) {
                    if (three.tally < three.numConnections) {
                if (isConnected(seven, three) == 1) {
                    connectRooms(&seven, &three);
                }
            }
            }

            if (findConn == 4) {
                    if (four.tally < four.numConnections) {
                if (isConnected(seven, four) == 1) {
                    connectRooms(&seven, &four);
                }
                    }
            }

            if (findConn == 5) {
                    if (five.tally < five.numConnections) {
                if (isConnected(seven, five) == 1) {
                    connectRooms(&seven, &five);
                }
            }
            }

            if (findConn == 6) {
                    if (six.tally < six.numConnections) {
                if (isConnected(seven, six) == 1) {
                    connectRooms(&seven, &six);
                }
            }
            }

            if (findConn == 1) {
                    if (one.tally < one.numConnections) {
                if (isConnected(seven, one) == 1) {
                    connectRooms(&seven, &one);
                }
                    }
            }
        }
    }


    // at the end of this, one should have between 3 and 6 connections
    printf("%s (7) connections are:\n", seven.name);

    for (i = 0; i < seven.numConnections; i++) {
        if (seven.connections[i] == NULL) {
            printf("Null value where you shouldn't have one\n");
        }
        else {
            printf("%s\n", seven.connections[i]->name);
        }
    }
}

int main(void) {
    srand(time(NULL));      // seed rand
    // The first thing your program must do is generate 7 different room files, one room per file, in a directory called <username>.rooms.<process id>
    generateRoomFiles();

    return 0;
}
