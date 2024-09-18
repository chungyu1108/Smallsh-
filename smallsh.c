#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#define LENGTH      2048
#define PROCESSES   1000
#define ARGUMENTS   512

struct sigaction SIGINTAction;	// SIGINT handler
struct sigaction SIGTSTPAction; // SIGTSTP handler
/********************************************************
*function: getInput
*parameters, including an array of character pointers (arr[]), 
an integer pointer (background), and two character arrays 
(inputName[] and outputName[]).

*The input is then tokenized using strtok with a space 
delimiter. The tokens are checked for the presence of 
the "<" or ">" character, which indicates the input or 
output file, respectively. If the "<" character is found, 
the next token is copied to inputName. If the ">" character 
is found, the next token is copied to outputName. Otherwise, 
the token is stored in the arr array using strdup.
********************************************************/
void getInput(char* arr[], int* count, char inputName[], char outputName[], int x)
{
    char input[LENGTH];
    int i, j;

    //Get input 
    printf(": ");
    fflush(stdout); // fixed function name

    fgets(input, LENGTH, stdin);

    //Remove newline
    int found = 0;
    for (i = 0; !found && i < LENGTH; i++)
    {
        if(input[i] == '\n')
        {
            input[i] = '\0';
            found = 1;
        }
    }

    //Translate rawInput into individual strings
    const char space[2] = " ";
    char* token = strtok(input, space);

    while(token != NULL) // changed for loop to while loop
    {
        if(!strcmp(token, "<"))
        {
            token = strtok(NULL, space);
            strcpy(inputName, token);
        }
        else if(!strcmp(token, ">")) // changed else to else if
        {
            token = strtok(NULL, space);
            strcpy(outputName, token); // fixed variable name
        }
        else if(!strcmp(token, "&")) // added handling for background process
        {
            *count = 1;
            break; // exit the loop if background process flag is set
        }
        else
        {
            arr[i] = strdup(token);
            for (j = 0; arr[i][j]; j++)
            {
                if(arr[i][j] == '$' && arr[i][j+1] == '$')
                {
                    arr[i][j] = '\0';
                    snprintf(arr[i], 256, "%s%d", arr[i], x);
                }
            }
            i++;
        }
        token = strtok(NULL, space);
    }
}

/********************************************************
** function: exeCommand
**
********************************************************/
void exeCommand(char* arr[], int* E_Status, struct sigaction sa, int count, char input_name[], char output_name[])
{
    int input, output, result;
    pid_t pid;

    pid = fork();
    switch(pid)
    {
        case -1:
            perror("fork");
            exit(1);
            break;

        case 0:
            sa.sa_handler = SIG_DFL;
            sigaction(SIGINT, &sa, NULL);

            if(strcmp(input_name, ""))
            {
                input = open(input_name, O_RDONLY);
                if(input == -1) {
                    perror("input file open");
                    exit(1);
                }
                result = dup2(input, 0);
                if(result == -1) {
                    perror("dup2 input");
                    exit(2);
                }
                close(input);
            }

            if(strcmp(output_name, ""))
            {
                output = open(output_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if(output == -1) {
                    perror("output file open");
                    exit(1);
                }
                result = dup2(output, 1);
                if(result == -1) {
                    perror("dup2 output");
                    exit(2);
                }
                close(output);
            }

            execvp(arr[0], arr);
            perror("execvp");
            exit(2);
            break;

        default:
            if(count == 0) {
                waitpid(pid, E_Status, 0);
            }
            break;
    }
}
/********************************************************
** function: getSIGTSTP
**When the SIGTSTP signal is received by the program, 
the getSIGTSTP function is called. If the num variable 
is currently set to 1, the function sets it to 0 and 
writes a message to standard output indicating that the 
program is now in "foreground-only mode". If num is 
currently set to 0, the function sets it to 1 and writes 
a message to standard output indicating that the program 
is now exiting "foreground-only mode".
********************************************************/
int num = 1;
void getSIGTSTP(int s)
{
    if(num == 1)
    {//if 1, set it to 0 and display a message
        char* temp = "Enter foreground-only mode(& is now ignored\n";
        write(1, temp, 49);
        fflush(stdout);
        num = 0;
    }
    else
    {//if is 0, set it to 1 and display message
        char* temp = "Exiting foreground-only mode\n";
        write(1, temp, 29);
        fflush(stdout);
        num = 1;
    }
}

/********************************************************
** function: cd_call
**A function is a block of code that performs a specific 
task. It can take input parameters, and may return a 
value or perform an action. Functions are used to organize 
code, make it reusable, and simplify complex operations 
by breaking them down into smaller, more manageable parts.
********************************************************/
int num2 = 0;
char curr[100]; // change type to char array
char* List[ARGUMENTS];

void cd_call() 
{
    int x = 0;
    if(num2 == 1) {
        x = chdir(getenv("HOME"));	
    } else {
        x = chdir(List[1]);		
    }
	
    if(x == 0) {
        printf("%s\n", getcwd(curr, 100)); // pass pointer to curr
    } else {
        printf("chdir() failed\n");
    }
    fflush(stdout);
}
/********************************************************
** function: status_call
**If the child process terminated normally, the exit status 
is obtained and stored in the err variable. If the child 
process was terminated by a signal, the signal number is 
obtained and stored in the sig variable. The function then 
calculates the exit value based on the values of errHold 
and sigHold. If the program exited normally, the exit status 
is printed to standard output, otherwise the signal number 
is printed and the errSignal pointer is set to 1. Finally, 
the standard output stream is flushed.
********************************************************/
void status_call(int* e_Signal) 
{
    int processStatus;
	int err = 0, sig = 0, exit;
    // Check the status of the last process
	waitpid(getpid(), &processStatus, 0);		
    // Return the status of the normally terminated child
	if(WIFEXITED(processStatus)) 
        err = WEXITSTATUS(processStatus);	
    // Return the status of an abnormally terminated child
    if(WIFSIGNALED(processStatus)) 
        sig = WTERMSIG(processStatus);		

    exit = err + sig == 0 ? 0 : 1;

    if(sig == 0) 
    	printf("exit value %d\n", exit);
    else {
    	*e_Signal = 1;
    	printf("terminated by signal %d\n", sig);
    }
    fflush(stdout);
}
/********************************************************
** function: exitstatus
*If the WIFEXITED macro evaluates to true, it means the 
child process has terminated normally and WEXITSTATUS 
provides the exit status of the process. The function 
then prints the exit status using printf

*If WIFEXITED is false, it means that the child process 
has been terminated by a signal. The function uses the 
WTERMSIG macro to obtain the signal number that caused 
the termination and prints a message that includes this 
signal number using printf.
********************************************************/
void exitstatus(int exit)
{
    if(WIFEXITED(exit))
    {//exited by status
        printf("exit value %d\n", WEXITSTATUS(exit)); 
    }
    else
    {//terminated by signal
        printf("terminated by signal %d\n", WTERMSIG(exit));
    }
}


int main()
{
    char* arguments[ARGUMENTS];
    char input[LENGTH];
    char output[LENGTH];
    int background = 0;
    int status = 0;
    int i = 0;
    int x = getpid();

    SIGINTAction.sa_handler = SIG_IGN;
    sigfillset(&SIGINTAction.sa_mask);
    SIGINTAction.sa_flags = 0;
    sigaction(SIGINT, &SIGINTAction, NULL);

    SIGTSTPAction.sa_handler = getSIGTSTP;
    sigfillset(&SIGTSTPAction.sa_mask);
    SIGTSTPAction.sa_flags = SA_RESTART;
    sigaction(SIGTSTP, &SIGTSTPAction, NULL);

while(1)
{
    memset(input, '\0', LENGTH);
    memset(output, '\0', LENGTH);
    int count = 0;
    background = 0;

    getInput(arguments, &count, input, output, x);

    if(arguments[0] == NULL) // If the input is empty, continue the loop
    {
        continue;
    }

    //Check for built-in command
    if(strcmp(arguments[0], "exit") == 0)
    {
        exit(0);
    }
    else if(strcmp(arguments[0], "cd") == 0)
    {
        if(arguments[1] == NULL)
        {
            chdir(getenv("HOME"));
        }
        else
        {
            chdir(arguments[1]);
        }
    }
    else if(strcmp(arguments[0], "status") == 0)
    {
        if(WIFEXITED(status)) // check if the child process terminated normally
        {
            printf("Exit status %d\n", WEXITSTATUS(status));
            fflush(stdout);
        }
        else if(WIFSIGNALED(status)) // check if the child process was terminated by a signal
        {
            printf("Terminated by signal %d\n", WTERMSIG(status));
            fflush(stdout);
        }
    }
    else // execute non-built-in command
    {
        // handle background process if the flag is set
        if(count == 1)
        {
            background = 1;
        }
        pid_t pid;
        int status;
        pid = fork();

        switch(pid)
        {
            case -1:
                perror("fork");
                exit(1);
                break;

            case 0:
                // Child process
                exeCommand(arguments, &status, SIGINTAction, background, input, output);
                break;

            default:
                // Parent process
                if(background == 0)
                {
                    waitpid(pid, &status, 0);
                }
                else
                {
                    printf("Background process with ID %d started.\n", pid);
                    fflush(stdout);
                }
                break;
            }
        }
    }
    return 0;
}
