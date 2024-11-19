//author Tze Kheng Goh
//process.h

#ifndef PROCESS_H 

#define PROCESS_H

#include <string.h>
#include <csse2310a3.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "commands.h"
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <limits.h>

#define UQCMP_ARR_SIZE 3 
#define NUM_PIPE_ENDS 8
#define NUM_OF_PROCESS 4
#define JOB_T_BUF 50  //buffer big enough to store a prefix for uqcmp. 

//used for signal handling. Volatile teller compiler that this boolean value
//may change during run time. Variable is static as all c files are 
//able to access this variable (probably a bad thing). 
static volatile bool interruptsig = false; 

//struct that keep tracks of job tested and the number of jobs passed. 
typedef struct AllJobs { 
    int passed; 
    int tested;
} Jobs;

//a struct that keep tracks of child pids and its exit value. 
typedef struct AllProcesses {
    pid_t processprogin; 
    pid_t processdemoin; 
    pid_t uqcmpout; 
    pid_t uqcmperr;

    int exitin; 
    int exitin2;
    int outcmp; 
    int errcmp;
} Process; 

//enum for exit statuses to reduce need for magic numbers. 
enum ExitStatus { 
    EXECFAIL = 98, PIPEFAIL = 99, CORRECTEXIT = 0, NOINFO = 3, UNEXPECTED = 4, 
    DIFF = 5, 
};

//enum for pipe ends to reduce need for magic numbers in code. 
enum PipeEnds { 
    READOUT1 = 0, WRITEOUT1 = 1, READOUT2 = 2, WRITEOUT2 = 3, READERROR1 = 4, 
    WRITEERROR1 = 5, READERROR2 = 6, WRITEERROR2 = 7
};

//enum for processes to avoid the use of magic numbers in code. 
enum Processes { 
    TESTPROGRAM = 0, SOLUTION = 1, TESTSTANDARDOUT = 2, TESTSTANDARDERROR = 3  
}; 

//handler for when a SigInt has been received. 
void handler(int sig); 

//Process all necessary arguments before the piping and forking stage.
Jobs* process_args(Commands* com, char* progname, bool* quietCommand, 
        bool* parallelflag, Jobs* track);  

//Will be used to handle sleep when a SigINT has been sent. If there is stil 
//remaining time, the program will sleep for that remaining time before
//exiting. 
void handle_sleep(struct timespec* remtime);

//function used to pipe all pipes needed to process demo-program, program and 
//uqcmp
int* pipe_fds(void); 

//Used to process standard input from job file and feed it to the demo-program
//and the program specified on the command line. 
void process_standard_input(int* pipes, int* fd3, int* fd4, char** toexec, 
        char* progname, char* filejob, int* count); 

//return an array that holds a struct of pids and their exit values
Process process_forks(char* filejob, char** startargs, int numofstartargs, 
        char* progname, int* jobnumber, bool* quietcom, Process children); 

//Report the testjobs based on the exit status of the child processes. 
int report_testjob(Process pids, int* passedtests, int* jobnum); 

//Send a sigkill to all process ids and reap them with a wait call. 
Process kill_reap_process(Process allpid); 

//Used to do the comparison on fd 3 and fd 4
void process_uqcmp(int* pipes, int* fd3, int* fd4, int* tracker, int* jobnum, 
        bool* quietCommand); 

//Function used to close all fild descriptors to ensure there are no opened
//file descriptors left.
void close_all_fds(int* pipes, int* fd3, int* fd4);

#endif
