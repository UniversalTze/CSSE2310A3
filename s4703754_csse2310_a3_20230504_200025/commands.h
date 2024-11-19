//author Tze Kheng Goh
//commands.h

#ifndef COMMANDS_H 

#define COMMANDS_H

#define JOB_BUF 5

#include <stdio.h>
#include <stdlib.h>
#include <csse2310a3.h>

//struct for valid commands.
typedef struct PosCommands { 
    int numoflines; 
    char** filejobs; 
} Commands; 

//function used to initalise and malloc space for struct items. 
Commands* init_command(void);

//Function identifer for process_jobfile. 
//Will check if jobs in jobfile are valid.
void process_jobfile(FILE* jobfile, char* filename, Commands* jobcom); 

//Function identifier for check_syntax. 
//Ensure that line is syntatically valid. (One comma and a jobfile is located 
//before the comma. 
char** check_syntax(char* line, int linecount, Commands* com, char* fname, 
        FILE* jobfile, char* ogline);

//checks if input file in jobfile is able to be read. 
//Will exit if file is invalid. 
void check_inputfile(char** jobline, int counter, Commands* com, char* fname,
        FILE* jobfile, char* line, char* duppedline);

//Will print the appropriate messages if jobfile is empty. Will also 
//exit with correct exit status and free necessary messages. 
void process_emptyfile_err(Commands* com, FILE* jobfile, char* fname);

//free all memory that has been allocated from the Commands struct.
void free_command(Commands* com);

#endif
