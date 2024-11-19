//Author Tze Kheng Goh 
//For processes and forking

#include "process.h"

/* handler()
 * ---------
 * 
 * Will set the static volatile boolean to true if a sig Int has been sent 
 * from the keyboard. 
 *
 * sig: The integer number that represents signal interrupt. 
 *
 * Returns: void
 *
 **/ 
void handler(int sig) { 
    interruptsig = true; 
    
}

/* process_args()
 * --------------
 * 
 * Function is will be the control flow for handling and sending jobs to be
 * processed sequentially or in parallel. This function will print the 
 * message: 
 * "Starting Job T", where T is replaced by job number (separated by new line)
 * It will send the necessary information to process_forks() and will 
 * process the job file sequentially or in parallel depending on boolean 
 * values. Function will return a pointer to a Jobs struct that holds the 
 * number of tests passed and the number of tests tested. 
 *
 * com: Pointer to Command struct that holds the list of jobs needed to 
 * be processed and the number of jobs in list. 
 * progname: Pointer to a string, which represents the desired program to be 
 * tested. 
 * quietCommand: Boolean pointer to quiet flag. True, if "--quiet" appears 
 * on the command line, else false. 
 * parallelflag: Boolean pointer to parallel flag. True if "--parallel" 
 * appears on the command line, else false. 
 * track: A pointer to a Job struct that holds the number of jobs tested and 
 * the number of jobs passed. 
 *
 * Returns: A pointer to a Job struct which will have updated values, 
 * depending on how many jobs were tested and how many jobs were 
 * passed. 
 *
 **/
Jobs* process_args(Commands* com, char* progname, bool* quietCommand, 
        bool* parallelflag, Jobs* track) { 
    int jobnumber = 1; 
    int index = 0;  
    Process alljobs[com->numoflines]; 
    struct timespec sleeptime = {2, 0}; //initial 2 second sleep 
    //initialise remaining time to zero, 
    struct timespec remainingtime = {0}; 

    Process children = {0}; //Set all values of a struct to 0.   
    while (index < com->numoflines && !interruptsig) { 
        //dup so original line is unaffected. 
        char* lines = strdup(com->filejobs[index]);
        char** job = split_line(lines, ','); 
        char* jobname = job[0]; 
        int numtok;
        char** startargs = split_space_not_quote(job[1], &numtok);  
        printf("Starting job %d\n", jobnumber);
        fflush(stdout);

        children = process_forks(jobname, startargs, numtok, progname, 
                &jobnumber, quietCommand, children);  
                free(job); 
        free(lines);  //don't need to read from this anymore
        free(startargs); 
        if (!(*parallelflag)) { 
            //for sequential jobs 
            nanosleep(&sleeptime, &remainingtime);
            handle_sleep(&remainingtime); 
            children = kill_reap_process(children);
            track->passed = report_testjob(children, &track->passed, 
                    &jobnumber); 
       } else { alljobs[index] = children; }
        index++;
        jobnumber++; 
        track->tested++; 
    }
    if (*parallelflag) { 
        // process parallel jobs 
        nanosleep(&sleeptime, &remainingtime); 
        handle_sleep(&remainingtime); 
        int jnumber = 1; 
        for (int counter = 0; counter < com->numoflines; counter++) {
            children = kill_reap_process(alljobs[counter]); 
            track->passed = report_testjob(children, &track->passed, &jnumber);
            jnumber++;
        } 
    }
    return track; 
}

/* handle_sleep() 
 * --------------
 *
 * Function will handle sleep when a sig Int has been sent from the keyboard. 
 * If there is still remaining sleep time, this function will make the 
 * program sleep for the remaining time before allowing it to print
 * the final messages before exiting. 
 * 
 * remtime: A pointer to a timespec struct that indicates how much sleep time 
 * left. Includes seconds and nano secconds of potential sleep time. 
 *
 * Returns: void
 *
 * Errors: Seg fault if pointer is pointing to a null struct. 
 *
 **/
void handle_sleep(struct timespec* remtime) { 
    struct timespec checktime = {0};
    if (remtime->tv_sec > 0 || remtime->tv_nsec > 0) { 
        nanosleep(remtime, &checktime); 
        handle_sleep(&checktime); 
        //ensures program will sleep for 2 seconds no matter how 
        //many sigINT's were sent
    }
}

/* pipe_fds()
 * ----------
 *
 * Function will create all the needed pipes for processing standard input, 
 * and the comparisons on uqcmp. After piping the file descriptors, it 
 * will add it to an int array in this order: read and write end of standard 
 * out 1, read and write end of standard out 2, read and write end of 
 * standard error 1 and read and write end of standrard error 2. 
 *
 * Returns: A malloc'ed int array of file descriptors. 
 *
 * Errors: Exits with a status of 99 if pipe fails.
 **/
int* pipe_fds(void) {   
    //malloc space to hold all the pipe ends
    int* allpipes = malloc(sizeof(int) * NUM_PIPE_ENDS); 

    //create the pipes
    int sdout1[2]; 
    int sdout2[2]; 
    int sderr1[2];
    int sderr2[2]; 

    if (pipe(sdout1) == -1 || pipe(sdout2) == -1 || pipe(sderr1) == -1 || 
            pipe(sderr2)) { 
        //pipe fail
        perror("Pipes"); 
        exit(PIPEFAIL); 
    }
    // Add all pipe ends to the int array holding the pipes. 
    for (int index = 0; index < NUM_PIPE_ENDS; index++) { 
        if (index < READOUT2) { 
            allpipes[index] = sdout1[index]; 
        }
        if (index < READERROR1) { 
            allpipes[index] = sdout2[index]; 
        }
        if (index < READERROR2) { 
            allpipes[index] = sderr1[index]; 
        }
        if (index < NUM_PIPE_ENDS) { 
            allpipes[index] = sderr2[index]; 
        }    
    }
    
    return allpipes; 
}

/* process_standard_input()
 * ------------------------
 *
 * This function will exec the program specified on command line and the 
 * demo-uqwordiply program. The output of standard output and error from 
 * these function will be the input to uqcmp. The inputs will be taken from 
 * the first argument in each line of the jobfile. Assocaited stater args
 * will be executed with the inputfile. 
 *
 * pipes: A pointer to an int array that holds a malloc'ed array of file 
 * descriptors. 
 * fd3: A pointer to file descriptor used to preserve file descriptor number 
 * 3. 
 * fd4: A pointer to an opened file descriptor to preserve file descriptor 
 * number 4. 
 * toexec: An array of strigs that will be executed with execvp()
 * progname: The program to be execvp(). Will be the input program from 
 * the command line. 
 * filejob: The name of the inputfile (first argument of each line 
 * in the valid jobfile array).
 * count: A pointer to an integer that keep tracks of where we are in 
 * terms of forking. 
 *
 * Returns: void
 *
 * Errors: Child process will exit with a status of 98 if execvp() fails. 
 * Seg fault, if any pointers in function parameters are NULL. 
 *
 **/
void process_standard_input(int* pipes, int* fd3, int* fd4, char** toexec, 
        char* progname, char* filejob, int* count) { 
    int outpipe, errpipe; 
    close(*fd3); 
    close(*fd4); //don't need these file descriptors in the child process.
    int filejobfd = open(filejob, O_RDONLY); 
    int index; 
    for (index = 0; index < NUM_PIPE_ENDS; index++) { 
        if ((*count % NUM_OF_PROCESS == TESTPROGRAM) && (index == WRITEOUT1 
                || index == WRITEERROR1)) { 
            //leave necessary files open for dup2() (output)
            outpipe = WRITEOUT1; 
            errpipe = WRITEERROR1; 
            continue; 
        }
        if ((*count % NUM_OF_PROCESS == SOLUTION) && (index == WRITEOUT2 || 
                index == WRITEERROR2)) { 
            //leave necessary files open for dup2() (error)
            outpipe = WRITEOUT2; 
            errpipe = WRITEERROR2; 
            continue; 
        }
        //close irrelevant pipes
        close(pipes[index]); 
    } 
    //dup2() the correct ends and close all original fds/pipeends. 
    dup2(filejobfd, STDIN_FILENO);   
    close(filejobfd);  
    dup2(pipes[outpipe], STDOUT_FILENO);    
    dup2(pipes[errpipe], STDERR_FILENO); 
    close(pipes[outpipe]); 
    close(pipes[errpipe]);

    if (*count % NUM_OF_PROCESS == TESTPROGRAM) {
        //used to determine which program to exec
        toexec[0] = progname; 
    } else {  
        toexec[0] = "demo-uqwordiply"; 
    }
    execvp(toexec[0], toexec); 
    exit(EXECFAIL); // if exec fails
}

/* process_forks()
 * ---------------
 * 
 * This is the control flow for the all the forks(). In this function, four
 * child processes are created. One child process will exec the programe 
 * specified on the command line. 
 * Another child process will exec the 
 * demo-uqwordiply program. 
 * The third child process will make a comparison on the standard output of 
 * both programs. 
 * The final child will make a comparison on the standard error of the both 
 * programs. This function will then return a process struct which will hold 
 * all the pids of the children process that are currently running. 
 * All file descriptors opened in this 
 * function will be closed. 
 *
 * filejob: The name of the input file given in the first argument of a valid
 * line in the jobfile. 
 * startargs: List of strings that contain the optional starter arguments to 
 * be executed with the program given on the command line and the demo. 
 * numstartargs: The number of optional arguments given in the job file. 
 * (second input separated by a comma). 
 * progname: The name of the program to be executed in a child process (given 
 * on the command line). 
 * jobnumber:Pointer to int that represents the job number to be processed. 
 * quietcom: A boolean ptr that is true if "--quiet" has been specified, else,
 * false. 
 * pids: Process struct with all values initialised to zero. 
 *
 * Returns: An updated process struct where all pids of child processes 
 * are stored. 
 *
 **/
Process process_forks(char* filejob, char** startargs, int numstartargs, 
        char* progname, int* jobnumber, bool* quietcom, Process pids) { 
    int fd3 = open("/dev/null", O_RDONLY); 
    int fd4 = open("/dev/null", O_RDONLY); 
    //preseve fds 3 and 4 without creating redundant files
    char* toexec[numstartargs + 1]; 
    for (int index = 0; index < numstartargs; index++) { 
        toexec[index + 1] = startargs[index]; 
    }
    toexec[numstartargs + 1] = NULL; 
    int* pipes = pipe_fds();  
    int tracker = 0;
    pid_t processin = fork();    
    if (!(processin)) {   
        process_standard_input(pipes, &fd3, &fd4, toexec, progname, filejob, 
                &tracker);  
    }
    ++tracker; 

    pid_t demoin = fork(); 
    if (!(demoin)) {   
        process_standard_input(pipes, &fd3, &fd4, toexec, progname, filejob, 
                &tracker); 
    }
    ++tracker; 
    pid_t cmpout = fork();  
    if (!(cmpout)) {      
        process_uqcmp(pipes, &fd3, &fd4, &tracker, jobnumber, quietcom);  
    }
    ++tracker; 
    pid_t cmperr = fork(); 
    if (!(cmperr)) { 
        process_uqcmp(pipes, &fd3, &fd4, &tracker, jobnumber, quietcom); 
    }
    //parent 
    close_all_fds(pipes, &fd3, &fd4);    
    pids.processprogin = processin; 
    pids.processdemoin = demoin; 
    pids.uqcmpout = cmpout; 
    pids.uqcmperr = cmperr; 
    return pids;
}

/* report_testjob()
 * ----------------
 * 
 * Function will report the result of the testjob based on the exit status of 
 * all the child processes in the Process struct. If any child process 
 * exits with an exec fail, it will print this message: 
 * "Job T unable to execute test", where T is Job number. 
 * If standard out differs it will print: 
 * "Job T Stdout differs". 
 * If standard out matches on both fds 3 and 4, it will print: 
 * "Job T Stdout matches". 
 * If standard error differs: 
 * "Job T Stderr differs".
 * If standard error matches: 
 * "Job T Stderr matches". 
 * If exit status of the demo program and program specified on command line
 * matches: 
 * "Exit status matches". 
 * If exit status of the demo program and other program are different:c 
 * "Exit status differs". 
 * All messages are split by a new line separator. 
 * It will also increment the number of tests passed. A test will pass if 
 * standard output matches, standard error matches and the exit status of
 * demo program and other program matches. 
 *
 *
 * allp: Processs struct that contains the exit status of each child process.
 * passedtests: An int pointer that points to a struct variable that evaluates 
 * how many tetsts has been passed. 
 * jobnum: A pointer to an integer that points to the number of jobs processed
 *
 * Returns: An integer that increments the number of tests passed, else it 
 * returns the same number. 
 *
 **/
int report_testjob(Process allp, int* passedtests, int* jobnum) {  
    int count = 0;
    int passed = *passedtests; //dereference the passedtests pointer
    if (WIFEXITED(allp.outcmp) || WIFEXITED(allp.errcmp)) { 
        if (WEXITSTATUS(allp.outcmp) == EXECFAIL || WEXITSTATUS(allp.errcmp) 
                == EXECFAIL || WEXITSTATUS(allp.exitin) == EXECFAIL || 
                WEXITSTATUS(allp.exitin2) == EXECFAIL) {
            printf("Job %d: Unable to execute test\n", *jobnum); 
            fflush(stdout);  
        } else { 
            //only print these messages if exec does not fail.
            if (WEXITSTATUS(allp.outcmp) == DIFF || WEXITSTATUS(allp.outcmp) 
                    == UNEXPECTED || WEXITSTATUS(allp.outcmp) == NOINFO) {  
                printf("Job %d: Stdout differs\n", *jobnum);  
                fflush(stdout);  
            }  
            if (WEXITSTATUS(allp.outcmp) == CORRECTEXIT) { 
                printf("Job %d: Stdout matches\n", *jobnum); 
                fflush(stdout); 
                count++; //increment counter if output matches
            }
            if (WEXITSTATUS(allp.errcmp) == DIFF || WEXITSTATUS(allp.errcmp) 
                    == UNEXPECTED || WEXITSTATUS(allp.errcmp) == NOINFO) {  
                printf("Job %d: Stderr differs\n", *jobnum); 
                fflush(stdout);
            } 
            if (WEXITSTATUS(allp.errcmp) == CORRECTEXIT) { 
                printf("Job %d: Stderr matches\n", *jobnum); 
                fflush(stdout); 
                count++; //increment counter if error matches
            }
            if (WEXITSTATUS(allp.exitin) ==  WEXITSTATUS(allp.exitin2)) { 
                printf("Job %d: Exit status matches\n", *jobnum); 
                fflush(stdout);
                count++; 
                //increment counter if exit status of both prog matches 
            }
            if (WEXITSTATUS(allp.exitin) != WEXITSTATUS(allp.exitin2)) { 
                printf("Job %d: Exit status differs\n", *jobnum); 
                fflush(stdout); 
            }
        }
    }
    if (count == 3) {
        passed++; //increment passed test if it passes all three tests
    }
    return passed;
}

/* kill_read_process()
 * -------------------
 * 
 * Function takes a Process struct which holds the process ids of the child
 * processes of the current job and 4 different int status that has been 
 * set to zero. This function will assign the exit status of each children 
 * process to each integer in the struct. It will do this after sending a 
 * kill signal to all child processes. 
 *
 * allp: Process struct that holds 4 different children processes and 4 
 * integers to keep track of childrens' exit status. 
 *
 * Returns: The original process struct with updated values. 
 *
 **/
Process kill_reap_process(Process allp) { 
    //send sigkill to all child process ids. 
    kill(allp.processprogin, SIGKILL); 
    kill(allp.processdemoin, SIGKILL); 
    kill(allp.uqcmpout, SIGKILL); 
    kill(allp.uqcmperr, SIGKILL);

    //reap all processes and assign exit statuses to struct variables
    waitpid(allp.processprogin, &allp.exitin, 0); 
    waitpid(allp.processdemoin, &allp.exitin2, 0); 
    waitpid(allp.uqcmpout, &allp.outcmp, 0); 
    waitpid(allp.uqcmperr, &allp.errcmp, 0); 
    return allp;
}

/* process_uqcmp()
 * ---------------
 *
 * This function will handle the comparison of information on file descriptors
 * 3 and 4. It will exec the program called "uqcmp" to compare standard
 * outputs and standard erros of two different programs. 
 * If "--quiet" has been specified on the command line, the function's output
 * will be sent to "/dev/null", else it will be printed to output. 
 * Process will exit with status 98 if exec fails. 
 *
 * pipes: A pointer to an int array that holds a malloc'ed array of file 
 * descriptors. 
 * fd3: A pointer to file descriptor used to preserve file descriptor number 
 * 3. 
 * fd4: A pointer to an opened file descriptor to preserve file descriptor 
 * number 4. 
 * tracker: A pointer to an integer that keep tracks of where we are in 
 * terms of forking. 
 * jobnum: A int pointer to the job number that we are processing. 
 * quietCommand: A boolean ptr that tells us if "--quiet" has been specified 
 * on the command line. 
 *
 * Returns: void 
 *
 * Errors: Process will exit with a status of 98 if exec fails. 
 *
 **/
void process_uqcmp(int* pipes, int* fd3, int* fd4, int* tracker, int* jobnum, 
        bool* quietCommand) { 
    int readpipe1, readpipe2; 
    for (int counter = 0; counter < NUM_PIPE_ENDS; counter++) { 
        if ((*tracker % NUM_OF_PROCESS == TESTSTANDARDOUT) && (counter == 
                READOUT1 || counter == READOUT2)) { 
            readpipe1 = READOUT1;  //leave open pipes needed for standard out
            readpipe2 = READOUT2; 
            continue; 
        }
        if ((*tracker % NUM_OF_PROCESS == TESTSTANDARDERROR) && (counter == 
                READERROR1 || counter == READERROR2)) { 
            readpipe1 = READERROR1; //assign and leave open pipes needed 
            readpipe2 = READERROR2; //for standard error. 
            continue; 
        }
        close(pipes[counter]); //close unnecessary pipes ends. 
    }

    dup2(pipes[readpipe1], *fd3); //dup output/error of one pipe end to fd3
    dup2(pipes[readpipe2], *fd4); //dup output/error of another pipe to fd4.  

    close(pipes[readpipe1]);  //close original pipes
    close(pipes[readpipe2]); 
    char* com2[UQCMP_ARR_SIZE]; 
    //assing a char array large enough to hold the prefix for uqcmp. 
    char jobnumber[JOB_T_BUF]; 
    com2[0] = "uqcmp"; 
    if (*tracker % NUM_OF_PROCESS == TESTSTANDARDOUT) {  
        //logic here used to determine the prefix for when uqcmp is
        //executed. 
        sprintf(jobnumber, "Job %d stdout", *jobnum);
        com2[1] = jobnumber; 
    } else { 
        sprintf(jobnumber, "Job %d stderr", *jobnum);
        com2[1] = jobnumber;
    }
    com2[2] = NULL; 
    if (*quietCommand) { 
        //supress output and error of uqcmp is "--quiet" is given on com line
        int blackhole = open("/dev/null", O_WRONLY); 
        dup2(blackhole, STDOUT_FILENO); 
        dup2(blackhole, STDERR_FILENO); 
        close(blackhole); 
    }
    execvp(com2[0], com2); 
    exit(EXECFAIL); // Exec failure
}

/* close_all_fds()
 * ---------------
 *
 * This function will close all file descriptors that have been opened in the 
 * parent process. 
 * 
 * allpipes: An array of file descriptors that correspond to read/write ends 
 * of 4 different pipes. 
 * fd3: Int pointer to a file descriptor that is open for reading. 
 * fd4: Int pointer to a file descriptor that is open for reading 
 *
 * Returns: void
 *
 **/
void close_all_fds(int* allpipes, int* fd3, int* fd4) { 
    for (int traverse = 0; traverse < NUM_PIPE_ENDS; traverse++) { 
        close(allpipes[traverse]); 
    }
    free(allpipes); //frees the malloc'ed int array. 
    close(*fd3); //close other opened fds.  
    close(*fd4); 
}


