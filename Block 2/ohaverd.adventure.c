/* Diana O'Haver
   Program 2
   ohaverd.adventure.c */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/* Define strings - since they won't change, global should be fine */

char error[] = "HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.";
char orange[] = "Orange";
char red[] = "Red";
char yellow[] = "Yellow";
char green[] = "Green";
char blue[] = "Blue";
char purple[] = "Purple";
char pink[] = "Pink";

/* enum room types for use later */
enum room_type {
    START_ROOM,
    MID_ROOM,
    END_ROOM
};

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

void generateRoomFiles() {
    int buffer = 30;
    int pid = getpid();
    char prefix[] = "ohaverd.rooms.";

    char *dirName = malloc(buffer);

    // mash everything together into one string called dirName
    snprintf(dirName, buffer, "%s%d", prefix, pid);
    printf("%s", dirName);          // debugging, remove later

    // example of creating a new directory if it doesn't already exist
    // sourced from: http://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
    struct stat st = {0};
    if (stat(dirName, &st) == -1) {         // if it is -1, then the directory doesn't already exist
        mkdir(dirName, 0755);              // 755 means that everyone can read and execute, but only owner can write
    }

    // then create each of the 7 files

}

int main(void) {

    // The first thing your program must do is generate 7 different room files, one room per file, in a directory called <username>.rooms.<process id>
    generateRoomFiles();


}
