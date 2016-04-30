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

/* Room types */
char start[] = "START_ROOM";
char mid[] = "MID_ROOM";
char end[] = "END_ROOM";

const int MAX_ROOMS = 7;
const int MIN_CONNS = 3;
const int MAX_CONNS = 6;

/* Each room of type struct to hold connections and file values */
typedef struct room room;               // so i can just declare rooms without struct, i guess i might care here

struct room {
    int roomNum;
    int tally;                          // tracking current number of connections
    int numConnections;                 // tracking total allowed connections
    room *connections[6];                // better - array of room connections so can be iterated through
    char name[20];                        // flexible array member is a terrible idea, use static value
    char roomtype[20];                   // type of room - moved to string instead of enum
};

/* Generate a random number between 3 and 6 to determine how many connections room will have */
int randNum() {
    return rand() % (6 + 1 - 3) + 3;
}

/* Generate random number between 0 and 9 to determine what the room's name is */
int randRoom() {
    return rand() % (9 + 0 - 0) + 0;
}

/* Generate random number between 0 and 6 to determine which room the room should be connected with */
int randConn() {
    return rand() % (6 + 0 - 0) + 0;
}

/* Generate random number between 0 and 6 to determine what the room type is */
int randType() {
    return rand() % (6 + 0 - 0) + 0;
}

/* This tests if room1 is already connected to room2 */
/* Return 0 for true, 1 for false */
int isConnected(room *room1, room *room2) {
    int i;
    // if they are connected - shouldn't need this by the end of the very manual connection process
    if (room1->name == room2->name) {
        return 0;
    }

    // loop through connections and make sure they aren't connected
    for (i = 0; i < room1->numConnections; i++) {
        if (room1->connections[i] != NULL) {                                 // don't bother checking null connections
           // printf("conns[i].name is %d\n", room1.connections[i]->roomNum);

            if (room1->connections[i]->roomNum == room2->roomNum) {                    // if they match
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
    int i, k, j;
    int findName, findType;
    room *activeRooms[7];               // array of pointers to room to  more easily loop through rooms
    int startFlag = 0;                  // track if start room has been taken yet
    int endFlag = 0;                    // track if end room has been taken yet

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

    // put the room type options into an array of pointers so the same will work
    char *roomTypes[7];
    roomTypes[0] = start;
    roomTypes[1] = mid;
    roomTypes[2] = mid;
    roomTypes[3] = mid;
    roomTypes[4] = mid;
    roomTypes[5] = mid;
    roomTypes[6] = end;

    for (i = 0; i < 7; i++) {
        // allocate memory for the room
        activeRooms[i] = (struct room *) malloc(sizeof(struct room));

        findName = randRoom();

        // find the name of the room
        for (k = 0; k < 10; k++) {
            if (k == findName) {
              while (roomNames[k] == NULL) {
                if (k == 10) {
                    k = 0;
                }
                else {
                    k++;
                }
            }
            strcpy(activeRooms[i]->name, roomNames[k]);
            // set that place to null so it won't be used again
            roomNames[k] = NULL;
            break;
            }
        }

        // set the room number
        activeRooms[i]->roomNum = i;

        // set the tally of connections so far
        activeRooms[i]->tally = 0;

        // find the number of connections the room can have
        activeRooms[i]->numConnections = randNum();

        findType = randType();

        // assign what room type the room is

        if (findType == 0 && startFlag == 0) {
            strcpy(activeRooms[i]->roomtype, start);
            startFlag = 1;
        }

        else if (findType == 6 && endFlag == 0) {
            strcpy(activeRooms[i]->roomtype, end);
            endFlag = 1;
        }

        else {
            strcpy(activeRooms[i]->roomtype, mid);
        }
//        for (j = 0; j < 7; j++) {
//            printf("j is %d\n", j);
//            if (j == findType) {
//                while (roomTypes[j] == NULL) {
//                    if (j == 7) {
//                        j = 0;
//                    }
//                    else {
//                        j++;
//                    }
//                }
//                printf("Assigning roomType of %s to %s\n", roomTypes[j], activeRooms[i]->name);
//
//                strcpy(activeRooms[i]->roomtype, roomTypes[j]);
//                //printf("j is %d\n", j);
//                // set that place to null so it won't be used again
//                roomTypes[j] = NULL;
//                break;
//            }
//        }
    }



    for (i = 0; i < 7; i++) {
        printf("%s has %d connections\n", activeRooms[i]->name, activeRooms[i]->numConnections);
        printf("%s is of room type %s\n", activeRooms[i]->name, activeRooms[i]->roomtype);
    }

    // now there are seven rooms named one through seven
    // set up the connections

    int findConn;

    // loop through connections and make sure that they're null for everyone before we start making connections

    for (i = 0; i < 7; i++) {
        makeNull(activeRooms[i]);
    }

    // now set up the connections

//    for (i = 0; i < 7; i++) {
//        for (k = 0; k < activeRooms[i]->numConnections; k++) {
//            while (activeRooms[i]->connections[k] == NULL) {
//                findConn = randConn();
//
//                if (findConn != i) {
//                    if (isConnected(activeRooms[i], activeRooms[findConn]) == 1) {
//                    connectRooms(activeRooms[i], activeRooms[findConn]);
//                    }
//                }
//            }
//        }
//    }
//
//    for (i = 0; i < 7; i++) {
//        printf("Connections for %s: \n", activeRooms[i]->name);
//
//        for (k = 0; k < activeRooms[i]->numConnections; k++) {
//            printf("%s\n", activeRooms[i]->connections[k]->name);
//        }
//    }
//
//    /** Hook up room one **/
//    for (i = 0; i < one.numConnections; i++) {
//
//        while (one.connections[i] == NULL) {
//            findConn = randConn();
//
//            if (findConn == 2) {
//                if (two.tally < two.numConnections) {
//                    if (isConnected(one, two) == 1) {
//                        connectRooms(&one, &two);
//                    }
//                }
//            }
//
//            if (findConn == 3) {
//                if (three.tally < three.numConnections) {
//                    if (isConnected(one, three) == 1) {
//                        connectRooms(&one, &three);
//                    }
//                }
//            }
//
//            if (findConn == 4) {
//                if (four.tally < four.numConnections) {
//                    if (isConnected(one, four) == 1) {
//                        connectRooms(&one, &four);
//                    }
//                }
//            }
//
//            if (findConn == 5) {
//                 if (five.tally < five.numConnections) {
//                    if (isConnected(one, five) == 1) {
//                        connectRooms(&one, &five);
//                    }
//                }
//            }
//
//            if (findConn == 6) {
//                if (six.tally < six.numConnections) {
//                    if (isConnected(one, six) == 1) {
//                        connectRooms(&one, &six);
//                    }
//                }
//            }
//
//            if (findConn == 7) {
//                if (seven.tally < seven.numConnections) {
//                    if (isConnected(one, seven) == 1) {
//                        connectRooms(&one, &seven);
//                    }
//                }
//            }
//        }
//
//
//    }
//
//
//
//    // at the end of this, one should have between 3 and 6 connections
//    printf("%s (1) connections are:\n", one.name);
//
//    for (i = 0; i < one.numConnections; i++) {
//        if (one.connections[i] == NULL) {
//            printf("Null value where you shouldn't have one\n");
//        }
//        else {
//            printf("%s\n", one.connections[i]->name);
//        }
//    }
//
//    /** Hook up room two **/
//    for (i = 0; i < two.numConnections; i++) {
//
//        while (two.connections[i] == NULL) {
//            findConn = randConn();
//
//            if (findConn == 1) {
//                if (one.tally < one.numConnections) {
//                    if (isConnected(two, one) == 1) {
//                        connectRooms(&two, &one);
//                    }
//                }
//            }
//
//            if (findConn == 3) {
//                if (three.tally < three.numConnections) {
//                if (isConnected(two, three) == 1) {
//                    connectRooms(&two, &three);
//                }
//            }
//            }
//
//            if (findConn == 4) {
//                if (four.tally < four.numConnections) {
//                    if (isConnected(two, four) == 1) {
//                        connectRooms(&two, &four);
//                    }
//                }
//            }
//
//            if (findConn == 5) {
//                if (five.tally < five.numConnections) {
//                if (isConnected(two, five) == 1) {
//                    connectRooms(&two, &five);
//                }
//                }
//            }
//
//            if (findConn == 6) {
//                if (six.tally < six.numConnections) {
//                if (isConnected(two, six) == 1) {
//                    connectRooms(&two, &six);
//                }
//                }
//            }
//
//            if (findConn == 7) {
//                if (seven.tally < seven.numConnections) {
//                if (isConnected(two, seven) == 1) {
//                    connectRooms(&two, &seven);
//                }
//                }
//            }
//        }
//    }
//
//    printf("%s (2) connections are:\n", two.name);
//
//    for (i = 0; i < two.numConnections; i++) {
//        if (two.connections[i] == NULL) {
//            printf("Null value where you shouldn't have one\n");
//        }
//        else {
//            printf("%s\n", two.connections[i]->name);
//        }
//    }
//
//    /** Hook up room three **/
//    for (i = 0; i < three.numConnections; i++) {
//
//        while (three.connections[i] == NULL) {
//            findConn = randConn();
//
//            if (findConn == 2) {
//                if (two.tally < two.numConnections) {
//                if (isConnected(three, two) == 1) {
//                    connectRooms(&three, &two);
//                }
//                }
//            }
//
//            if (findConn == 1) {
//                    if (one.tally < one.numConnections) {
//                if (isConnected(three, one) == 1) {
//                    connectRooms(&three, &one);
//                }
//                    }
//            }
//
//            if (findConn == 4) {
//                    if (four.tally < four.numConnections) {
//                if (isConnected(three, four) == 1) {
//                    connectRooms(&three, &four);
//                }
//                    }
//            }
//
//            if (findConn == 5) {
//                    if (five.tally < five.numConnections) {
//                if (isConnected(three, five) == 1) {
//                    connectRooms(&three, &five);
//                }
//                    }
//            }
//
//            if (findConn == 6) {
//                    if (six.tally < six.numConnections) {
//                if (isConnected(three, six) == 1) {
//                    connectRooms(&three, &six);
//                }
//                    }
//            }
//
//            if (findConn == 7) {
//                    if (seven.tally < seven.numConnections) {
//                if (isConnected(three, seven) == 1) {
//                    connectRooms(&three, &seven);
//                }
//                    }
//            }
//        }
//    }
//
//    printf("%s (3) connections are:\n", three.name);
//
//    for (i = 0; i < three.numConnections; i++) {
//        if (three.connections[i] == NULL) {
//            printf("Null value where you shouldn't have one\n");
//        }
//        else {
//            printf("%s\n", three.connections[i]->name);
//        }
//    }
//
//    /** Hook up room four **/
//    for (i = 0; i < four.numConnections; i++) {
//
//        while (four.connections[i] == NULL) {
//            findConn = randConn();
//
//            if (findConn == 2) {
//                    if (two.tally < two.numConnections) {
//                if (isConnected(four, two) == 1) {
//                    connectRooms(&four, &two);
//                }
//            }
//            }
//
//            if (findConn == 3) {
//                    if (three.tally < three.numConnections) {
//                if (isConnected(four, three) == 1) {
//                    connectRooms(&four, &three);
//                }
//            }
//            }
//
//            if (findConn == 1) {
//                    if (one.tally < one.numConnections) {
//                if (isConnected(four, one) == 1) {
//                    connectRooms(&four, &one);
//                }
//                    }
//            }
//
//            if (findConn == 5) {
//                    if (five.tally < five.numConnections) {
//                if (isConnected(four, five) == 1) {
//                    connectRooms(&four, &five);
//                }
//                    }
//            }
//
//            if (findConn == 6) {
//                    if (six.tally < six.numConnections) {
//                if (isConnected(four, six) == 1) {
//                    connectRooms(&four, &six);
//                }
//                    }
//            }
//
//            if (findConn == 7) {
//                    if (seven.tally < seven.numConnections) {
//                if (isConnected(four, seven) == 1) {
//                    connectRooms(&four, &seven);
//                }
//            }
//            }
//        }
//    }
//
//    printf("%s (4) connections are:\n", four.name);
//
//    for (i = 0; i < four.numConnections; i++) {
//        if (four.connections[i] == NULL) {
//            printf("Null value where you shouldn't have one\n");
//        }
//        else {
//            printf("%s\n", four.connections[i]->name);
//        }
//    }
//
//    /** Hook up room five **/
//    for (i = 0; i < five.numConnections; i++) {
//
//        while (five.connections[i] == NULL) {
//            findConn = randConn();
//
//            if (findConn == 2) {
//                    if (two.tally < two.numConnections) {
//                if (isConnected(five, two) == 1) {
//                    connectRooms(&five, &two);
//                }
//                    }
//            }
//
//            if (findConn == 3) {
//                    if (three.tally < three.numConnections) {
//                if (isConnected(five, three) == 1) {
//                    connectRooms(&five, &three);
//                }
//            }
//            }
//
//            if (findConn == 4) {
//                    if (four.tally < four.numConnections) {
//                if (isConnected(five, four) == 1) {
//                    connectRooms(&five, &four);
//                }
//            }
//            }
//
//            if (findConn == 1) {
//                    if (one.tally < one.numConnections) {
//                if (isConnected(five, one) == 1) {
//                    connectRooms(&five, &one);
//                }
//                    }
//            }
//
//            if (findConn == 6) {
//                    if (six.tally < six.numConnections) {
//                if (isConnected(five, six) == 1) {
//                    connectRooms(&five, &six);
//                }
//                    }
//            }
//
//            if (findConn == 7) {
//                    if (seven.tally < seven.numConnections) {
//                if (isConnected(five, seven) == 1) {
//                    connectRooms(&five, &seven);
//                }
//                }
//            }
//        }
//    }
//
//    printf("%s (5) connections are:\n", five.name);
//
//    for (i = 0; i < five.numConnections; i++) {
//        if (five.connections[i] == NULL) {
//            printf("Null value where you shouldn't have one\n");
//        }
//        else {
//            printf("%s\n", five.connections[i]->name);
//        }
//    }
//
//    /** Hook up room six **/
//    for (i = 0; i < six.numConnections; i++) {
//
//        while (six.connections[i] == NULL) {
//            findConn = randConn();
//
//            if (findConn == 2) {
//                    if (two.tally < two.numConnections) {
//                if (isConnected(six, two) == 1) {
//                    connectRooms(&six, &two);
//                }
//                    }
//            }
//
//            if (findConn == 3) {
//                    if (three.tally < three.numConnections) {
//                if (isConnected(six, three) == 1) {
//                    connectRooms(&six, &three);
//                }
//            }
//            }
//
//            if (findConn == 4) {
//                    if (four.tally < four.numConnections) {
//                if (isConnected(six, four) == 1) {
//                    connectRooms(&six, &four);
//                }
//            }
//            }
//
//            if (findConn == 5) {
//                    if (five.tally < five.numConnections) {
//                if (isConnected(six, five) == 1) {
//                    connectRooms(&six, &five);
//                }
//            }
//            }
//
//            if (findConn == 1) {
//                    if (one.tally < one.numConnections) {
//                if (isConnected(six, one) == 1) {
//                    connectRooms(&six, &one);
//                }
//            }
//            }
//
//            if (findConn == 7) {
//                    if (seven.tally < seven.numConnections) {
//                if (isConnected(six, seven) == 1) {
//                    connectRooms(&six, &seven);
//                }
//                    }
//            }
//        }
//    }
//
//    printf("%s (6) connections are:\n", six.name);
//
//    for (i = 0; i < six.numConnections; i++) {
//        if (six.connections[i] == NULL) {
//            printf("Null value where you shouldn't have one\n");
//        }
//        else {
//            printf("%s\n", six.connections[i]->name);
//        }
//    }
//
//    /** Hook up room seven **/
//    for (i = 0; i < seven.numConnections; i++) {
//
//        while (seven.connections[i] == NULL) {
//            findConn = randConn();
//
//            if (findConn == 2) {
//                    if (two.tally < two.numConnections) {
//                if (isConnected(seven, two) == 1) {
//                    connectRooms(&seven, &two);
//                }
//                    }
//            }
//
//            if (findConn == 3) {
//                    if (three.tally < three.numConnections) {
//                if (isConnected(seven, three) == 1) {
//                    connectRooms(&seven, &three);
//                }
//            }
//            }
//
//            if (findConn == 4) {
//                    if (four.tally < four.numConnections) {
//                if (isConnected(seven, four) == 1) {
//                    connectRooms(&seven, &four);
//                }
//                    }
//            }
//
//            if (findConn == 5) {
//                    if (five.tally < five.numConnections) {
//                if (isConnected(seven, five) == 1) {
//                    connectRooms(&seven, &five);
//                }
//            }
//            }
//
//            if (findConn == 6) {
//                    if (six.tally < six.numConnections) {
//                if (isConnected(seven, six) == 1) {
//                    connectRooms(&seven, &six);
//                }
//            }
//            }
//
//            if (findConn == 1) {
//                    if (one.tally < one.numConnections) {
//                if (isConnected(seven, one) == 1) {
//                    connectRooms(&seven, &one);
//                }
//                    }
//            }
//        }
//    }
//
//
//    // at the end of this, one should have between 3 and 6 connections
//    printf("%s (7) connections are:\n", seven.name);
//
//    for (i = 0; i < seven.numConnections; i++) {
//        if (seven.connections[i] == NULL) {
//            printf("Null value where you shouldn't have one\n");
//        }
//        else {
//            printf("%s\n", seven.connections[i]->name);
//        }
//    }
}

int main(void) {
    srand(time(NULL));      // seed rand
    // The first thing your program must do is generate 7 different room files, one room per file, in a directory called <username>.rooms.<process id>
    generateRoomFiles();

    return 0;
}
