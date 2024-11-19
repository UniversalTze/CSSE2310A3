//a3 file
//author: Tze Kheng Goh 
// Lets get cooking

#include <stdbool.h>
#include <string.h>
#include "commands.h"
#include "process.h"

#define LENGTH_NO_OP_COM 3
#define LENGTH_TWO_OP_COM 5
#define OUT_OF_BOUNDS 6
#define FIND_JOB_FILE 1
#define FIND_PROGRAM 2


void handler(int sig); 
void check_starter_args(int argc, char** argv, int* progindex, int* jobpos); 
bool check_three_args(int argc, char** argv); 
bool check_opcoms_args(int argc, char** argv, int* progindex, int* jobpos); 
FILE* check_jobfile(int argc, char** argv, int* jobindex); 
void check_flags(int argc, char** argv, bool* quietflag, bool* parallelflag); 

/* Main function
 *
 * Controls main flow of program. 
 */
int main(int argc, char** argv) {  
    int progindex = argc - FIND_PROGRAM;
    int jobindex = argc - FIND_JOB_FILE;
    struct sigaction actionsig; 
    memset(&actionsig, 0, sizeof(actionsig));
    actionsig.sa_handler = handler; 
    actionsig.sa_flags = SA_RESTART;
    sigaction(SIGINT, &actionsig, NULL);  
    //*actionsig will replace old behaviour of sigint.
    //*oldact in sigaction is null as don't need to save current handler/  

    check_starter_args(argc, argv, &progindex, &jobindex); 
    FILE* job = check_jobfile(argc, argv, &jobindex); 
    bool quietcheck, parallelcheck;

    quietcheck = parallelcheck = false; //set quite and parallel flag to false
    bool* quietflag = &quietcheck; 
    bool* parallel = &parallelcheck; 
    check_flags(argc, argv, quietflag, parallel); 
    //will set quietflag/parllel if they appear on command line. 

    Commands* coms = init_command(); //initialise command struct
    process_jobfile(job, argv[argc - 1], coms); //initial validity checks. 

    Jobs track = {0}; //initialise all struct variables to zero. 

    Jobs* tracker = process_args(coms, argv[progindex], quietflag, parallel, 
            &track);
    printf("testuqwordiply: %d out of %d tests passed\n", tracker->passed, 
            tracker->tested);

    if (tracker->passed != tracker->tested) { 
        //exit with a status of one, if a test was failed
        free_command(coms); 
        exit(1); 
    }
    free_command(coms); 
    exit(0);
}

/* check_starter_args() 
 * --------------------
 *
 * Function will check if the parameters given on the command line are valid. 
 * Ensures that the number of parameters entered is between 3 and 5.
 * If not, it will print this message: 
 * "Usage: testuqwordiply [--quiet] [--parallel] testprogram jobfile". 
 * Separated by a new line separator. 
 * It will also exit with a status of 2. 
 *
 * argc: The number of arguments given on the command line. 
 * argv: The list of arguments entered in on the command line. 
 * progindex: pointer to integer that represents the index of program file 
 * on the command line. 
 * jobpos: pointer to integer that represents the index of jobfile on the 
 * command line. 
 *
 * Returns: void
 *
 */ 
void check_starter_args(int argc, char** argv, int* progindex, int* jobpos) { 
    bool invalid = false; 
    if (argc == LENGTH_NO_OP_COM) { 
        //expects no optional commands. 
        invalid = check_three_args(argc, argv); 
    } 
    if (LENGTH_NO_OP_COM < argc && argc < OUT_OF_BOUNDS) { 
        //expects one or both optional commands. 
        invalid = check_opcoms_args(argc, argv, progindex, jobpos);
    } 
    if (invalid || argc < LENGTH_NO_OP_COM || argc > LENGTH_TWO_OP_COM) {
        fprintf(stderr, "Usage: testuqwordiply [--quiet] [--parallel] "
                "testprogram jobfile\n"); 
        exit(2); 
    }
}

/* check_three_args() 
 * ------------------
 *
 * Function will check the validity of command line when given only 
 * 3 commands. Checks that no optional command was given. Also ensures that 
 * all arguments given don't have "--". Will return a boolean value. If 
 * arguments given on the command line is invalid, it will return true. 
 * Else, false. 
 * 
 * argc: The number of arguments given on the command line. 
 * argv: The list of arguments entered in on the command line. 
 *
 * Returns: True, if arguments given begin with "--", else false. 
 *
 */
bool check_three_args(int argc, char** argv) {  
    char* check = "--"; 
    for (int loop = 1; loop < argc; loop++) {  
        //Ensures that any commands given don't begin with "--". 
        if (strstr(argv[loop], check) != NULL) { 
            return true; 
        }
    }
    return false;
}

/* check_opcom_args()
 * ------------------
 *
 * Function checks the validity of command line when more than 3 arguments 
 * have been entered. Thus, it will check for optional commands. Function 
 * will return true if command line is invalid. Else, it will return a 
 * boolean value of false. The command line is invalid when: 
 * optional command has been entered twice; 
 * an argument begins with "--" but is not an optional command; 
 * if optional commands populate the last 2 spots of argv.
 *
 * argc: The number of arguments given on the command line. 
 * argv: The list of arguments entered in on the command line. 
 * progindex: pointer to integer that represents the index of program file 
 * on the command line. 
 * jobpos: pointer to integer that represents the index of jobfile on the 
 * command line. 
 *
 * Returns: True if arguments entered in on command line are invalid, else 
 * false. 
 *
 */
bool check_opcoms_args(int argc, char** argv, int* progindex, int* jobpos) { 
    char* optionalCom1 = "--quiet"; 
    char* optionalCom2 = "--parallel"; 
    char* check = "--"; 

    for (int counter = 1; counter < *progindex; counter++) { 
        if (strcmp(argv[counter], optionalCom1) && 
                strcmp(argv[counter], optionalCom2)) { 
            //check if first two arguments is not "--quiet" and 
            //"--parallel". 
            return true; 
        }
        int checkindex = counter;
        checkindex--; 
        char* opCom = argv[checkindex];
        if (!strcmp(argv[counter], opCom)) { 
            //checks for duplicates of optional commands. 
            return true; 
        }
    }

    if (strstr(argv[*progindex], check) != NULL) { 
        return true; 
        //checks if the "--" substring appears in program index on C line. 
    }
    
    if (strstr(argv[*jobpos], check) != NULL) { 
        //checks if the "--" substring appears in job index on C line
        return true; 
    }
    return false;
}

/* check_jobfile() 
 * ---------------
 *
 * This function will check the job file validity. It will make sure that the
 * job file exits and that it can be opened for reading. If the jobfile 
 * does not exist or cannot be opened for reading, the program will print this
 * message: 
 * "testuqwordiply: Unable to open jobfile "jobname"". 
 * Separated by a new line separator. 
 * It will also exit with a status of 3. 
 *
 * argc: The number of arguments given on the command line. 
 * argv: The list of arguments entered in on the command line. 
 * jobindex: Pointer to integer that represents the position job on the 
 * command line. 
 *
 * Returns: A pointer to an opened job file that is ready to be read from. 
 *
 */
FILE* check_jobfile(int argc, char** argv, int* jobindex) { 
    FILE* commands = fopen(argv[*jobindex], "r"); 
    if (commands == NULL) { 
        fprintf(stderr, "testuqwordiply: Unable to open jobfile "); 
        fprintf(stderr, "\"%s\"\n", argv[*jobindex]); 
        exit(3); 
    }
    return commands; 
}

/* check_flags()
 * -------------
 *
 * Function will check if the quiet or parallel optional command has been 
 * entered in on the command line. If "--quiet" appears on the command line, 
 * function will set a quiet boolean ptr to true. If "--parallel" is given 
 * on the command line, it will set a parallel boolean ptr to true. Else, 
 * it will do nothing. 
 *
 * argc: The number of arguments given on the command line. 
 * argv: The list of arguments entered in on the command line. 
 * quietflag: boolean ptr that points to a boolean that has been currently 
 * set to false. 
 * parallelflag: boolean ptr that points to a boolean that has been currently 
 * set to false. 
 *
 * Returns: void
 * 
 * Errors: Seg fault may be caused if the boolean ptr is pointing to an 
 * uninitialised boolean value. 
 *
 */
void check_flags(int argc, char** argv, bool* quietflag, bool* parallelflag) {
    char* quietcom = "--quiet"; 
    char* parallelcom = "--parallel";
    for (int index = 0; index < argc; index++) { 
        if (strcmp(quietcom, argv[index]) == 0) { 
            //if "--quiet" appears on command line
            *quietflag = true; 
        }
        if (strcmp(parallelcom, argv[index]) == 0) { 
            //if "--parallel" appears om command line
            *parallelflag = true;
        }
    }

}
