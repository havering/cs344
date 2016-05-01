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
char found[] = "YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!";
char youtook[] = "YOU TOOK";
char path[] = "STEPS. YOUR PATH TO VICTORY WAS:";

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

/* Function to actually play and output game */
void playGame(room activeRooms[]) {
    int i;
    room startRoom;
    room endRoom;
    room currentRoom;           // wherever the user is currently
    int steps = 0;
    char pathtaken[200];        // string to store path taken by user

    strcpy(pathtaken, "\n");    // so strcat can be used later down the line

    // first figure out which room is the start room
    for (i = 0; i < 7; i++) {
        if (strcmp(activeRooms[i].roomtype, start) == 0) {
            startRoom = activeRooms[i];
        }
    }

    // then figure out the end room
    for (i = 0; i < 7; i++) {
        if (strcmp(activeRooms[i].roomtype, end) == 0) {
            endRoom = activeRooms[i];
        }
    }

    currentRoom = startRoom;

    while (currentRoom.roomNum != endRoom.roomNum) {
        // print out room name
        printf("\n%s %s\n", curr, currentRoom.name);

        // attach possible connections to a string
        char poscon[100];

        // char to read user input
        char whereto[20];

        // 3 connections
        if (currentRoom.numConnections == 3) {
            strcpy(poscon, currentRoom.connection1);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection2);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection3);
            strcat(poscon, ".");
        }

        // four connections
        if (currentRoom.numConnections == 4) {
            strcpy(poscon, currentRoom.connection1);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection2);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection3);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection4);
            strcat(poscon, ".");
        }

        // five connections
        if (currentRoom.numConnections == 5) {
            strcpy(poscon, currentRoom.connection1);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection2);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection3);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection4);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection5);
            strcat(poscon, ".");
        }

        // six connections
        if (currentRoom.numConnections == 6) {
            strcpy(poscon, currentRoom.connection1);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection2);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection3);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection4);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection5);
            strcat(poscon, ", ");
            strcat(poscon, currentRoom.connection6);
            strcat(poscon, ".");
        }

        // print out possible connections
        printf("%s %s\n", poss, poscon);

        // print out prompt for new location
        printf("%s", where);

        // read in input from user
        scanf("%s", &whereto);

        for (i = 0; i < 7; i++) {
            if (strcmp(whereto, activeRooms[i].name) == 0) {
                currentRoom = activeRooms[i];
                // increment steps because we've found a valid room to move to
                steps++;

                // add room name to path
                strcat(pathtaken, currentRoom.name);
                strcat(pathtaken, "\n");
                break;
            }
            if ((i == 6) && (strcmp(whereto, activeRooms[i].name) != 0)) {
                printf("\n%s\n", error);
            }
        }
    }
    // if it gets here, the user has found the end room
    printf("%s\n", found);
    printf("%s %d %s", youtook, steps, path);
    printf("%s", pathtaken);



}

/* Function to generate room files, placed in an array */
void generateRoomFiles(char *dirName) {
    int i, k, findConn, j;
    room activeRooms[7];               // array of rooms to  more easily loop through rooms

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

				else if (activeRooms[i].tally == 1){
					strcpy(activeRooms[i].connection2, activeRooms[findConn].name);
				}

				else if (activeRooms[i].tally == 2){
					strcpy(activeRooms[i].connection3, activeRooms[findConn].name);
				}

				else if (activeRooms[i].tally == 3){
					strcpy(activeRooms[i].connection4, activeRooms[findConn].name);
				}

				else if (activeRooms[i].tally == 4){
					strcpy(activeRooms[i].connection5, activeRooms[findConn].name);
				}
				else if (activeRooms[i].tally == 5){
					strcpy(activeRooms[i].connection6, activeRooms[findConn].name);
				}

				// reciprocal connections
				if(activeRooms[findConn].tally == 0){
					strcpy(activeRooms[findConn].connection1, activeRooms[i].name);
				}
				else if (activeRooms[findConn].tally == 1){
					strcpy(activeRooms[findConn].connection2, activeRooms[i].name);
				}
				else if (activeRooms[findConn].tally == 2){
					strcpy(activeRooms[findConn].connection3, activeRooms[i].name);
				}
				else if (activeRooms[findConn].tally == 3){
					strcpy(activeRooms[findConn].connection4, activeRooms[i].name);
				}
				else if (activeRooms[findConn].tally == 4){
					strcpy(activeRooms[findConn].connection5, activeRooms[i].name);
				}
				else if (activeRooms[findConn].tally == 5){
					strcpy(activeRooms[findConn].connection6, activeRooms[i].name);
				}

				// increase tally
				activeRooms[i].tally++;
				activeRooms[findConn].tally++;

            }

            finder = 0;
        }

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
    //closedir(dirName); this isn't working idk

    // now time to play the game
    playGame(activeRooms);
}


int main(void) {
    srand(time(NULL));      // seed rand
    int buffer = 30;
    char *dirName = malloc(buffer);


    makeDirectory(dirName);

    generateRoomFiles(dirName);

    return 0;
}
