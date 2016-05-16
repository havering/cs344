// Diana O'Haver
// smallsh
// Initial framework of shell adapted from: http://brennan.io/2015/01/16/write-a-shell-in-c/
// Program should:
//   - support exit, cd, and status
//   - allow redirection of stdin and stdout
//   - support foreground and background processes
//   - colon symbol is prompt
//   - command line with max length of 2048 characters
//   - command line with max of 512 arguments
//   - allow blank lines and comments (lines that begin with #)

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUFFER 2048
#define MAX_ARGS 512

// globals
int exitStatus = 0;                 // declaring this as a global so it doesn't need to be passed into each function

// function prototypes
void changeDir(char **args);
int findStatus(char **args);
int exitProg(char **args);

// called when user wants to change directory
// example of changing directory in C: http://stackoverflow.com/questions/1293660/is-there-any-way-to-change-directory-using-c-language
void changeDir(char **args) {
    char* newDir;
    // if no second argument, cd should take user back to home variable
    // example of returning to home using getenv: http://www.tutorialspoint.com/c_standard_library/c_function_getenv.htm
    if (args[1] == NULL) {
        newDir = getenv("HOME");
    }
    // if there is a second argument, this is where we want to go to
    else {
        newDir = args[1];
    }
    if (chdir(newDir) == -1) {
        printf("no such file or directory\n");
        exitStatus = 1;             // return 1 on error
    }
}

// function to return the status code of most recently exited process
int getStatus(char **args) {
    printf("exited with status %d\n", exitStatus);
    // reset so valid exits are recorded correctly
    exitStatus = 0;
    return 1;
}

// function to exit program since Ctrl-C is not allowed
int exitProg(char **args)
{
  exit(0);
}

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("blap");
      printf("error in execvp child process\n");
      exitStatus = 1;
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("bloop");
    printf("error in forking (pid < 0)\n");
    exitStatus = 1;
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

// function to test that cd is actually working
// example of pwd implementation: http://stackoverflow.com/questions/16285623/pwd-to-get-path-to-the-current-file
void mypwd(char **args) {
    char cwd[1024];

    getcwd(cwd, sizeof(cwd));
    printf("Current working directory: %s\n", cwd);
}

// function to read in input from the user
// example reading input using getline: http://c-for-dummies.com/blog/?p=1112
char* getInput() {
    char *buffer;
    size_t bufsize = INPUT_BUFFER;          // max size of assignment specs

    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }

    getline(&buffer, &bufsize, stdin);

    // remove random newline that keeps showing up??
    int length = strlen(buffer);
    buffer[length-1] = '\0';

    return buffer;
}

int executeArgs(char **args) {
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    // if user wants to cd somewhere
    if (strcmp(args[0], "cd") == 0) {
        changeDir(args);
        return 1;
    }

    // if user wants to exit
    if (strcmp(args[0], "exit") == 0) {
        exitProg(args);
    }

    // if user enters a comment
    if (strcmp(args[0], "#") == 0) {
        return 1;
    }

    // if user wants to know the status
    if (strcmp(args[0], "status") == 0) {
        getStatus(args);
        return 1;
    }

    // if user wants to know the current working directory
    if (strcmp(args[0], "pwd") == 0) {
        mypwd(args);
        return 1;
    }
    return lsh_launch(args);
    //return 1;
}

// function to split the input line and evaluate what the user wants to do
// example of splitting a string by token: http://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
char** splitArgs(char *line) {
    int i = 0;
    char* token;    // hold each token as it is read in
    // need malloc in order to return local variable - see: http://stackoverflow.com/questions/12380758/c-error-function-returns-address-of-local-variable
    char** commandArray = (char**)malloc(sizeof(char*) * MAX_ARGS);

    token = strtok(line, " ");

    while (token != NULL) {
        commandArray[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    commandArray[i] = NULL;
    return commandArray;
}



void runProg(void) {
  char *line;
  char **args;
  int status;

  do {
    printf(": ");                   // print prompt
    line = getInput();              // read line from command line input
    args = splitArgs(line);    // split line into arguments
    status = executeArgs(args);     // execute the arguments

    free(line);
    free(args);
  } while (status);
}



int main(int argc, char **argv) {

  runProg();

  return 0;
}
