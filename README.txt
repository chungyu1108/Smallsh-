******************************************************
General information
******************************************************
// Program: assingment2 movie.c
// Author: chung-yu, yang
// Date: 2/10/2023
// Description: 
1. Provide a prompt for running commands
Handle blank lines and comments, which are lines beginning with the # character
Provide expansion for the variable $$
Execute 3 commands exit, cd, and status via code built into the shell
Execute other commands by creating new processes using a function from the exec family of functions
Support input and output redirection
Support running commands in foreground and background processes
Implement custom handlers for 2 signals, SIGINT and SIGTSTP

OUTPUT:
$ ./p3testscript 2>&1
or

$ ./p3testscript 2>&1 | more
or

$ ./p3testscript > mytestresults 2>&1 


******************************************************
Data and file overview
******************************************************
This code appears to be a C program that provides a basic 
shell interface, allowing the user to enter commands 
and execute them. The program reads input from the user 
using the fgets() function and tokenizes it using strtok(). 
The tokens are checked to see if they represent input or 
output redirection, as well as if the process should be 
run in the background. If the token is not a redirection 
or background token, it is stored in an array of character 
pointers.

******************************************************
Sharing and access information
******************************************************
The getInput function takes in user input, tokenizes it 
using strtok, and then checks for special characters <, >, 
and & which indicate input redirection, output redirection, 
and background process execution respectively. The function 
then populates arrays arr, inputName, and outputName with 
the appropriate values.

The exeCommand function takes in the arr, E_Status, sa, 
count, input_name, and output_name parameters, creates 
a child process using fork, and then uses execvp to execute 
the command specified in arr. Input and output redirection 
is handled by opening files using open and using dup2 to 
set the standard input and output to the opened files.

The getSIGTSTP function is a signal handler for the SIGTSTP 
signal, which is sent when the user presses Ctrl-Z. When 
this signal is received, the function toggles a global 
variable num and displays a message indicating whether 
the program is entering or exiting "foreground-only mode".

The cd_call function changes the current working directory 
using chdir. If the num2 variable is set to 1, the function 
changes the directory to the user's home directory. Otherwise, 
it changes the directory to the path specified in the List 
array. The current working directory is then printed to 
standard output.

******************************************************
Methodological information
******************************************************
In the getInput function, the i variable that is used to 
index into the arr array is not initialized before it is 
used. It should be initialized to 0 before the loop that 
tokenizes the input string.

In the getInput function, the check for the background 
process flag (&) is not correctly implemented. If the & 
character is found, the loop should be exited immediately 
and the flag should be set. Here's the corrected code
 
******************************************************
Data-specific information
******************************************************
The code provided is a partial implementation of a shell 
program that can execute commands entered by the user. 
The program includes functions to get user input, execute
 a command, and handle signals.

The getInput() function reads user input and tokenizes 
it into separate strings. It also checks for input/output 
redirection and sets the inputName and outputName variables 
accordingly. If the command is meant to be executed in the 
background, it sets the background flag. The function also 
replaces any instances of $$ in the command with the process ID.

The exeCommand() function executes the command passed 
as an argument. It creates a child process using fork(), 
sets up input/output redirection if necessary, and executes 
the command using execvp(). If the background flag is not 
set, it waits for the child process to finish.

The getSIGTSTP() function is a signal handler for SIGTSTP. 
It toggles the num variable to indicate whether the program 
is in "foreground-only mode" or not.

The code is missing several important components of a shell 
program, including command history, piping, and environment 
variable handling. It also lacks error handling for certain 
system calls and could benefit from additional comments and 
documentation.