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
#include <dirent.h>

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
    char name[7];                        // flexible array member is a terrible idea, use static value
    char roomtype[11];                   // type of room - moved to string instead of enum
};

/* Generate a random number between 3 and 6 to determine how many connections room will have */
int randNum() {
    return rand() % (6 + 1 - 3) + 3;
}

/* Generate random number between 0 and 6 to determine which room the room should be connected with */
int randConn() {
    return rand() % 7;
}

/* This tests if room1 is already connected to room2 */
/* Return 0 for true, 1 for false */
int isConnected(room *room1, room *room2) {
    int i;

    // if they are connected
    if (strcmp(room1->name, room2->name) == 0) {
        return 0;
    }

    // loop through connections and make sure they aren't connected
    for (i = 0; i < room1->numConnections; i++) {
        if (room1->connections[i] != NULL) {                                 // don't bother checking null connections
           // printf("conns[i].name is %d\n", room1.connections[i]->roomNum);

            if (strcmp(room1->connections[i]->name, room2->name) == 0) {
//            if (room1->connections[i]->roomNum == room2->roomNum) {                    // if they match
                //printf("Returning 0 because %s is connected to %s\n", room1->name, room2->name);
                //printf("i is %d\n", i);
                //printf("%s has max %d allowed connections\n", room1->name, room1->numConnections);
                return 0;
            }
        }
    }
    return 1;               // if it makes it through the above and no match, return false
}

/* Connect two rooms together if possible */
void connectRooms(room *room1, room *room2) {
    int i;

    // check that connections aren't already maxed out
    if (room1->tally >= room1->numConnections) {
        return;
    }

    if (room2->tally >= room2->numConnections) {
        return;
    }

    // if it gets here, then we should be able to connect the rooms
    // zero indexing allows inserting the connection at the connCount
    room1->connections[room1->tally] = room2;
    room2->connections[room2->tally] = room1;

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

/* Function to randomly arrange values in array so we can skip the null assignment stuff */
// Shuffle sourced from: http://stackoverflow.com/questions/6127503/shuffle-array-in-c
void jumbler(char **nameArray, size_t size) {
    if (size > 1) {
        size_t i;
        for (i = 0; i < size - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (size - i) + 1);
          char* t = nameArray[j];
          nameArray[j] = nameArray[i];
          nameArray[i] = t;
        }
    }
}

/* Separating directory making into its own function for cleanliness */
void makeDirectory() {
    int buffer = 30;
    int pid = getpid();
    char prefix[] = "ohaverd.rooms.";

    char *dirName = malloc(buffer);

    // mash everything together into one string called dirName
    snprintf(dirName, buffer, "%s%d", prefix, pid);

    // example of creating a new directory if it doesn't already exist
    // sourced from: http://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
    struct stat st = {0};
    if (stat(dirName, &st) == -1) {         // if it is -1, then the directory doesn't already exist
        mkdir(dirName, 0755);              // 755 means that everyone can read and execute, but only owner can write
    }
    return;
}

/* Function to generate room files, placed in an array */
struct room generateRoomFiles() {
    int i, k;
    room activeRooms[7];               // array of pointers to room to  more easily loop through rooms

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

    // mix up the room names
    jumbler(roomNames, 10);

    // create an array to hold the first 7 shuffled room names
    char *usedRooms[7];

    // put the first 7 room names in the array
    for (i = 0; i < 7; i++) {
        usedRooms[i] = roomNames[i];
    }

    // mix up the room types
    jumbler(roomTypes, 7);

    for (i = 0; i < 7; i++) {
        // set the name of the room
        strcpy(activeRooms[i].name, usedRooms[i]);

        // set the room number
        activeRooms[i].roomNum = i;

        // set the tally of connections so far
        activeRooms[i].tally = 0;

        // find the number of connections the room can have
        activeRooms[i].numConnections = randNum();

        // set the type of the room
        strcpy(activeRooms[i].roomtype, roomTypes[i]);


    }



    for (i = 0; i < 7; i++) {
        printf("%s has %d connections\n", activeRooms[i].name, activeRooms[i].numConnections);
        printf("%s is of room type %s\n", activeRooms[i].name, activeRooms[i].roomtype);
    }

    // loop through connections and make sure that they're null for everyone before we start making connections

    for (i = 0; i < 7; i++) {
        makeNull(&activeRooms[i]);
    }

}


int main(void) {
    srand(time(NULL));      // seed rand

    makeDirectory();

    generateRoomFiles();

    return 0;
}
