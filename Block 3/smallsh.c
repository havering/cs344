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
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define INPUT_BUFFER 2048
#define MAX_ARGS 512

// globals
int exitStatus = 0;                 // declaring this as a global so it doesn't need to be passed into each function

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
        fflush(stdout);
        exitStatus = 1;             // return 1 on error
    }
}

// function to return the status code of most recently exited process
int getStatus(char **args) {

    // exiting with status 1 issue: http://stackoverflow.com/questions/3736320/executing-shell-script-with-system-returns-256-what-does-that-mean
    if (exitStatus == 256) {
        printf("exited with status 1\n");
    }
    else {
        printf("exited with status %d\n", exitStatus);
    }

    // reset so valid exits are recorded correctly
    exitStatus = 0;
    fflush(stdout);

    return 1;
}

// function to exit program since Ctrl-C is not allowed
int exitProg(char **args) {
    exit(0);
}

int ssh_launch(char **args) {
    pid_t pid, wpid, child_id;
    int status;
    int background = 0;             // hold whether or not the user has specified a background process
    int infile, outfile, in, out;
    int wantsInput = 0;             // 0 for false
    int wantsOutput = 0;            // 0 for false
    int printed = 0;

      // ignoring SIGINT signal in child process: http://stackoverflow.com/questions/12953350/ignore-sigint-signal-in-child-process
    struct sigaction action;
    action.sa_handler = SIG_IGN;          // ignore by default
    sigaction(SIGINT, &action, NULL);


    // find out how many commands the user entered
    int count = 0;

    while (args[count] != NULL) {
        count++;
    }

    // check if the user wants a background process
    if (strcmp(args[count-1], "&") == 0) {
        background = 1;             // user wants the args to be a background process

        args[count-1] = NULL;
    }

    // check for user wants to output
    // example of redirecting output to a file: http://stackoverflow.com/questions/8516823/redirecting-output-to-a-file-in-c
    // arguments for open: http://pubs.opengroup.org/onlinepubs/009695399/basedefs/fcntl.h.html

    int track = 0;

    while (args[track] != NULL) {
        if (strcmp(args[track], ">") == 0) {

            // 0600 is owner has rw, everyone else has nothing: https://codex.wordpress.org/Changing_File_Permissions
            outfile = open(args[track+1], O_RDWR|O_CREAT|O_APPEND, 0600);

            wantsOutput = 1;        // true

            // check for errors
            if (outfile == -1) {
                perror("opening output file");
                exitStatus = 1;
                return 1;                      // exit or else it gets stuck here
            }

            // set those spots in the argument to null so they don't get passed to exec
            // credit to Shoshana Abrass for the suggestion
            args[track] = NULL;
            args[track+1] = NULL;
        }

        // now same thing, but checking for input direction
        else if (strcmp(args[track], "<") == 0) {
             // input file only needs to be read_only
            if (args[track+1] != NULL) {
                infile = open(args[track+1], O_RDONLY);
            }
            // "Background commands should have their standard input redirected from /dev/null if the user did not specify some other file to take standard input from."
            // example at: http://stackoverflow.com/questions/14846768/in-c-how-do-i-redirect-stdout-fileno-to-dev-null-using-dup2-and-then-redirect
            else {
                infile = open("/dev/null", O_WRONLY);
            }


            wantsInput = 1;         // true

            // check for errors
            if (infile == -1) {
                perror("opening input file");
                exitStatus = 1;
                return 1;                  // exit or else it gets stuck here
            }

            args[track] = NULL;
            args[track+1] = NULL;
        }
        track++;
    }

    pid = fork();

    if (pid == 0) {                           // Child process
        if (background == 0) {                  // if it is not supposed to be a background process, it can be interrupted
            action.sa_handler = SIG_DFL;        // default value i.e. not ignore kill signals
            action.sa_flags = 0;
            sigaction(SIGINT, &action, NULL);
        }

        if (wantsOutput == 1) {
            // all i/o redirection must be with dup2 per specs
            // 1 is stdout
            out = dup2(outfile, 1);

            // check for errors
            if (out == -1) {
                perror("dup2 error: outputfile");
                exitStatus = 1;
            }
        }

        if (wantsInput == 1) {
            // all i/o redirection must be with dup2 per specs
            // 0 is stdin
            in = dup2(infile, 0);

            // check for errors
            if (in == -1) {
                perror("dup2 error: inputfile");
                exitStatus = 1;
            }
        }


        if (execvp(args[0], args) == -1) {
            perror("error");
            exitStatus = 1;
        }
        exit(EXIT_FAILURE);
  }

  else if (pid < 0) {
    // Error forking
    perror("error");
    exitStatus = 1;
  }

  else {


    // close in/outfiles
    if (wantsInput == 1) {
        close(infile);
    }

    if (wantsOutput == 1) {
        close(outfile);
    }

    // check if background has been chosen
    // if not, wait for process to complete before moving on
    if (background == 0) {
        wpid = waitpid(pid, &status, WUNTRACED);
        exitStatus = status;
    }
    // if it is a background process, per specs, print out the pid and return control to command line
    else {
        printf("background process is %d\n", pid);
    }
  }

  return 1;
}

// function to test that cd is actually working
// example of pwd implementation: http://stackoverflow.com/questions/16285623/pwd-to-get-path-to-the-current-file
void mypwd(char **args) {
    char cwd[1024];

    getcwd(cwd, sizeof(cwd));
    printf("Current working directory: %s\n", cwd);
    fflush(stdout);
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

    // if it gets here, then it's a non built in command and we can start forking
    return ssh_launch(args);
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

// function to check for child processes ending
void checkChildren() {
    int status;
    pid_t cpid;

    while ((cpid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("background process id is %d\n", cpid);

        if (WIFEXITED(status)) {
            printf("exited with status %d\n", status);
        }
        else if (WIFSIGNALED(status)) {
            printf("terminated with signal %d\n", WTERMSIG(status));
        }
    }
}

// main loop to run program
void runProg(void) {
  char *line;
  char **args;
  int status;


  do {
    checkChildren();                // check for ending child processes
    fflush(stdout);
    printf(": ");                   // print prompt
    fflush(stdout);
    line = getInput();              // read line from command line input
    args = splitArgs(line);         // split line into arguments
    status = executeArgs(args);     // execute the arguments

    free(line);
    free(args);
  } while (status);
}



int main(int argc, char **argv) {

  runProg();

  return 0;
}
