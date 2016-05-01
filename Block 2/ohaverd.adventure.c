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
    //room *connections[6];                // better - array of room connections so can be iterated through
    char connection1[20];
    char connection2[20];
    char connection3[20];
    char connection4[20];
    char connection5[20];
    char connection6[20];
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
//int isConnected(room *room1, room *room2) {
//    int i;
//
//    // if they are connected
//    if (strcmp(room1->name, room2->name) == 0) {
//        return 0;
//    }
//
//    // loop through connections and make sure they aren't connected
//    for (i = 0; i < room1->numConnections; i++) {
//        if (room1->connections[i] != NULL) {                                 // don't bother checking null connections
//           // printf("conns[i].name is %d\n", room1.connections[i]->roomNum);
//
//            if (strcmp(room1->connections[i]->name, room2->name) == 0) {
////            if (room1->connections[i]->roomNum == room2->roomNum) {                    // if they match
//                //printf("Returning 0 because %s is connected to %s\n", room1->name, room2->name);
//                //printf("i is %d\n", i);
//                //printf("%s has max %d allowed connections\n", room1->name, room1->numConnections);
//                return 0;
//            }
//        }
//    }
//    return 1;               // if it makes it through the above and no match, return false
//}

/* Connect two rooms together if possible */
//void connectRooms(room *room1, room *room2) {
//    int i;
//
//    // check that connections aren't already maxed out
//    if (room1->tally >= room1->numConnections) {
//        return;
//    }
//
//    if (room2->tally >= room2->numConnections) {
//        return;
//    }
//
//    // if it gets here, then we should be able to connect the rooms
//    // zero indexing allows inserting the connection at the connCount
//    room1->connections[room1->tally] = room2;
//    room2->connections[room2->tally] = room1;
//
//    room1->tally = room1->tally + 1;
//    room2->tally = room2->tally + 1;
//
//    return;
//}

/* Function to loop through and make sure everything is null */
void makeNull(room *inputRoom) {
    int i;

    for (i = 0; i < inputRoom->numConnections; i++) {
//        inputRoom->connection1 = NULL;
//        inputRoom->connection2 = NULL;
//        inputRoom->connection3 = NULL;
//        inputRoom->connection4 = NULL;
//        inputRoom->connection5 = NULL;
//        inputRoom->connection6 = NULL;
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
void makeDirectory(char *dirName) {
    int buffer = 30;
    int pid = getpid();
    char prefix[] = "ohaverd.rooms.";

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
void generateRoomFiles(char *dirName) {
    int i, k, findConn, j;
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
        //printf("%s has %d connections\n", activeRooms[i].name, activeRooms[i].numConnections);
        printf("%s is of room type %s\n", activeRooms[i].name, activeRooms[i].roomtype);
    }

    // loop through connections and make sure that they're null for everyone before we start making connections

//    for (i = 0; i < 7; i++) {
//        makeNull(&activeRooms[i]);
//    }

    // now activeRooms has 7 random rooms with random connections already assigned
    // write those newly created rooms to their respective files

    // open the directory first
    opendir(dirName);

    for (i = 0; i < 7; i++) {
        char tempString[100];

        // copy directory name over to string
        strcpy(tempString, dirName);

        // add a slash
        strcat(tempString, "/");

        // append the room name
        strcat(tempString, activeRooms[i].name);

        // creating/writing to a file in C: http://www.tutorialspoint.com/cprogramming/c_file_io.htm
        FILE *fp;

        fp = fopen(tempString, "w");

        // write room name as first step
        fprintf(fp, "ROOM NAME: %s\n", activeRooms[i].name);

        // time to build connections
        int finder = 0;


	while(activeRooms[i].tally < activeRooms[i].numConnections){
        	findConn = randConn();

        // check if connection already exists
		if(strcmp(activeRooms[i].connection1, activeRooms[findConn].name) == 0){
			finder = 1;
		}
		if(strcmp(activeRooms[i].connection2, activeRooms[findConn].name) == 0){
			finder = 1;
		}
		if(strcmp(activeRooms[i].connection3, activeRooms[findConn].name) == 0){
			finder = 1;
		}
		if(strcmp(activeRooms[i].connection4, activeRooms[findConn].name) == 0){
			finder = 1;
		}
		if(strcmp(activeRooms[i].connection5, activeRooms[findConn].name) == 0){
			finder = 1;
		}
		if(strcmp(activeRooms[i].connection6, activeRooms[findConn].name) == 0){
			finder = 1;
		}

		// check if it's connected to itself
		if(strcmp(activeRooms[i].name, activeRooms[findConn].name)==0){
			finder = 1;
		}

		// check that max connections haven't already been assigned for either room
		if (activeRooms[i].tally == 6){
			finder = 1;
		}
		if (activeRooms[findConn].tally == 6){
			finder = 1;
		}

        // if we get here and finder is still 0, then connection can be made
		if(finder != 1){
				// figure out which connection to store it in based on tally
				if(activeRooms[i].tally == 0){
					strcpy(activeRooms[i].connection1, activeRooms[findConn].name);
				}

				else if(activeRooms[i].tally == 1){
					strcpy(activeRooms[i].connection2, activeRooms[findConn].name);
				}

				else if(activeRooms[i].tally == 2){
					strcpy(activeRooms[i].connection3, activeRooms[findConn].name);
				}

				else if(activeRooms[i].tally == 3){
					strcpy(activeRooms[i].connection4, activeRooms[findConn].name);
				}

				else if(activeRooms[i].tally == 4){
					strcpy(activeRooms[i].connection5, activeRooms[findConn].name);
				}
				else if(activeRooms[i].tally == 5){
					strcpy(activeRooms[i].connection6, activeRooms[findConn].name);
				}

				// reciprocal connections
				if(activeRooms[findConn].tally == 0){
					strcpy(activeRooms[findConn].connection1, activeRooms[i].name);
				}
				else if(activeRooms[findConn].tally == 1){
					strcpy(activeRooms[findConn].connection2, activeRooms[i].name);
				}
				else if(activeRooms[findConn].tally == 2){
					strcpy(activeRooms[findConn].connection3, activeRooms[i].name);
				}
				else if(activeRooms[findConn].tally == 3){
					strcpy(activeRooms[findConn].connection4, activeRooms[i].name);
				}
				else if(activeRooms[findConn].tally == 4){
					strcpy(activeRooms[findConn].connection5, activeRooms[i].name);
				}
				else if(activeRooms[findConn].tally == 5){
					strcpy(activeRooms[findConn].connection6, activeRooms[i].name);
				}

				// increase tally
				activeRooms[i].tally++;
				activeRooms[findConn].tally++;

		}
		finder = 0;
	}

//            //printf("before for loop with j\n");
//            //printf("activerooms[i] conns is %d\n", activeRooms[i].numConnections);
//            // check if this is already a connection for the given room
//            for (j = 0; j < activeRooms[i].numConnections; j++) {
//                //printf("connections[j] roomnums is %d\n", activeRooms[i].connections[j]->roomNum);
//                //printf("findConn is %d\n", findConn);
//                //printf("activerooms[findcon] roomnums is %d\n", activeRooms[findConn].roomNum);
//                if (activeRooms[i].connections[j] != NULL) {
//                    if (activeRooms[i].connections[j]->roomNum == activeRooms[findConn].roomNum) {
//                    finder = 1;
//                    }
//                }
//
//            }
//            //printf("after for loop with j\n");
//            // check if it is connected to itself
//            if (activeRooms[i].roomNum == activeRooms[findConn].roomNum) {
//                finder = 1;
//            }
//
//            // check if there are open connections on given room
//            if (activeRooms[i].tally >= activeRooms[i].numConnections) {
//                finder = 1;
//            }
//
//            // check if there are open connections on randomly chosen room
//            if (activeRooms[findConn].tally >= activeRooms[findConn].numConnections) {
//                finder = 1;
//            }
//            //printf("before if finder != 1\n");
//            // if it gets here and found is still 0, it should be good to form a connection
//            if (finder != 1) {
//                // connect the room
//                activeRooms[i].connections[activeRooms[i].tally] = &activeRooms[findConn];
//
//                // increase tally
//                activeRooms[i].tally++;
//
//                // reciprocal connection
//                activeRooms[findConn].connections[activeRooms[findConn].tally] = &activeRooms[i];
//
//                // increase tally
//                activeRooms[findConn].tally++;
//            }
//            finder = 0;


        printf("Number of connections for %s: %d\n", activeRooms[i].name, activeRooms[i].numConnections);


        // then write connections to file
        // 3 connections
        if (activeRooms[i].tally == 3) {
            fprintf(fp, "CONNECTION1: %s\n", activeRooms[i].connection1);
            fprintf(fp, "CONNECTION2: %s\n", activeRooms[i].connection2);
            fprintf(fp, "CONNECTION3: %s\n", activeRooms[i].connection3);
        }
        // 4 connections
        if (activeRooms[i].tally == 4) {
            fprintf(fp, "CONNECTION1: %s\n", activeRooms[i].connection1);
            fprintf(fp, "CONNECTION2: %s\n", activeRooms[i].connection2);
            fprintf(fp, "CONNECTION3: %s\n", activeRooms[i].connection3);
            fprintf(fp, "CONNECTION4: %s\n", activeRooms[i].connection4);
        }
        // 5 connections
        if (activeRooms[i].tally == 5) {
            fprintf(fp, "CONNECTION1: %s\n", activeRooms[i].connection1);
            fprintf(fp, "CONNECTION2: %s\n", activeRooms[i].connection2);
            fprintf(fp, "CONNECTION3: %s\n", activeRooms[i].connection3);
            fprintf(fp, "CONNECTION4: %s\n", activeRooms[i].connection4);
            fprintf(fp, "CONNECTION5: %s\n", activeRooms[i].connection5);
        }
        // 6 connections
        if (activeRooms[i].tally == 6) {
            fprintf(fp, "CONNECTION1: %s\n", activeRooms[i].connection1);
            fprintf(fp, "CONNECTION2: %s\n", activeRooms[i].connection2);
            fprintf(fp, "CONNECTION3: %s\n", activeRooms[i].connection3);
            fprintf(fp, "CONNECTION4: %s\n", activeRooms[i].connection4);
            fprintf(fp, "CONNECTION5: %s\n", activeRooms[i].connection5);
            fprintf(fp, "CONNECTION6: %s\n", activeRooms[i].connection6);
        }


        // write the room type to the file
        fprintf(fp, "ROOM TYPE: %s\n", activeRooms[i].roomtype);

        fclose(fp);
    }

}


int main(void) {
    srand(time(NULL));      // seed rand
    int buffer = 30;
    char *dirName = malloc(buffer);


    makeDirectory(dirName);

    generateRoomFiles(dirName);

    return 0;
}
