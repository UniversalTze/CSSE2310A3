/* commands.c
 *
 * Functionality used to check the jobfile after intial checks 
 * 
 * author: Tze Kheng Goh
 *
 */

#include "commands.h"
#include <csse2310a3.h>
#include <string.h>
#include <stdbool.h>

/* init_command()
 *---------------
 *
 * Function will intiliase a pointer to a commands struct that will hold the 
 * number of lines in a job file and a list of strings which are lines in the
 * job files. 
 * 
 * Void arguments. 
 *
 * Returns: pointer to a Commands struct. 
 */
Commands* init_command(void) { 
    Commands* command = malloc(sizeof(Commands) * 1); 
    command->numoflines = 0;
    command->filejobs = malloc(sizeof(char*) * JOB_BUF); 
    return command;
}

/* process_jobfile()
 * -----------------
 *
 * Function will process the jobfile specified on the command line. 
 * Will ensure that each line is valid before adding it to the list of strings
 * (variable of command struct). 
 * Jobfile is only valid if all the lines in the file are valid. 
 * Will ignore any empty lines/ lines that begin with "#"
 *
 * jobfile: File pointer that points to the opened jobfile (specified on 
 * command line). 
 * filename: Is a char pointer to the name of the jobfile. 
 * jobcom: Pointer to a Commands struct that hold will list of commands (if 
 * valid from jobfile) and num of jobs in the list. 
 *
 * Returns: Void
 *
 * Errors: Seg fault if any pointers are NULL. 
 *
 */
void process_jobfile(FILE* jobfile, char* filename, Commands* jobcom) { 
    int count = 1; //line count starts at 1  
    char* line; 
    while ((line = read_line(jobfile)) != NULL) {   
        //skip line if it begins with # or empty line
        if (line[0] == '#' || line[0] == '\0') { 
            free(line);
            count++;
            continue;
        } 
        //process line if line is not empty or a comment  
        //syntax check job line 

        //Dupped original so original line is not affected when checking 
        char* lines = strdup(line); 

        char** jobline = check_syntax(lines, count, jobcom, filename, jobfile,
                line);
        //cheks input file after syntax is valid
        check_inputfile(jobline, count, jobcom, filename, jobfile, line, 
                lines); 
        //free jobline and lines now as no need to read from them anymore
        free(jobline);
        free(lines);

        //add original line to list after all valid checks
        jobcom->filejobs[jobcom->numoflines] = line;
        jobcom->numoflines++;

        if ((jobcom->numoflines > 0) && (jobcom->numoflines % JOB_BUF == 0)) { 
            jobcom->filejobs = realloc(jobcom->filejobs, sizeof(char*) 
                * (2 * jobcom->numoflines));
            //reallocate space for array if all memory malloc has been used. 
        }
        count++;
    }
    if (jobcom->numoflines == 0) { 
        //checks if job file given was empty
        process_emptyfile_err(jobcom, jobfile, filename); 
    }
    fclose(jobfile);
}

/* check_syntax() 
 * --------------
 *
 * Function will check if each line in the jobfile has valid syntax. A line in
 * the jobfile is valid if a input file has been specified, and there 
 * is only one comma separating jobfile and optional starter arguments. 
 * If the line's syntax is not valid, it will free all allocated memory 
 * and exit with a status of 4. It will also print this message: 
 * "testuqwordiply: syntax error on line linenum of "filename""
 *
 * line: The current line, from the jobfile to be processed.
 * (duplicate of original) 
 * linecount: The current line number in the jobfile. 
 * com: pointer to a Commands struct that hold will list of commands (if 
 * valid from jobfile) and num of jobs in the list. 
 * fname: File name of the jobfile being processed (specified on 
 * command line). 
 * jobfile: File pointer to jobfile (specified on command line(
 * that has been opened for reading. 
 * ogline: The original line in the jobfile. 
 *
 * Returns: List of strings that has an input file and optional 
 * starter arguments. 
 *
 * Errors: Will exit with a status 4 if syntax of line is invalid. 
 *
 */
char** check_syntax(char* line, int linecount, Commands* com, char* fname, 
        FILE* jobfile, char* ogline) { 
    char comma = ','; 
    bool invalid = false; 
    //checks if there is no comma in the line
    if (strchr(line, comma) == NULL) { 
        invalid = true; 
    } 
    int count; 
    int commcount = 0; 
    for (count = 0; line[count] != '\0'; count++) { 
        if (line[count] == comma) {
            commcount++; 
        }
    }
    //makes sure that there is only one comma in the line, else invalid 
    if (commcount != 1) { 
        invalid = true; 
    }
    //checks if a job file is specified. If not, line is invalid 
    char** jobline = split_line(line, ','); 
    if (jobline[0][0] == '\0') { 
        invalid = true; 
    }
    if (invalid) { 
        free(line);
        free(ogline);
        free(jobline);
        free_command(com); 
        fprintf(stderr, "testuqwordiply: syntax error on line %d of \"%s\"\n", 
                linecount, fname);
        fclose(jobfile);
        exit(4);
    }
    return jobline; 
}

/* check_inputfile() 
 * -----------------
 * 
 * Function will ensure that all input files (specified as first arguments in 
 * job files) exist and are able to be opened for reading. If not it will 
 * free all allocated memory and exit with a status of 5. It will also print 
 * this message before exit: 
 * "testuqwordiply: unable to open file "inputfile" specified on 
 * line linenum of "filename"". Separated by a new line seperator. 
 *
 * jobline: List of strings that hold the jobfile and optional starter
 * arguments. 
 * counter: Current line number being processed in the job file. 
 * com: pointer to a Commands struct that hold will list of commands (if 
 * valid from jobfile) and num of jobs in the list. 
 * fname: File name of the jobfile being processed (specified on 
 * command line). 
 * jobfile: File pointer to jobfile (specified on command line) that has been 
 * opened for reading. 
 * line: The original and current line in the jobfile. 
 * duppedline: Copy of original and current line in the jobfile. 
 *
 * Returns: void
 * 
 * Errors: Program will exit with a status of 5 if file cannot be opened for 
 * reading. 
 *
 */
void check_inputfile(char** jobline, int counter, Commands* com, char* fname,
        FILE* jobfile, char* line, char* duppedline) {  
    FILE* filename = fopen(jobline[0], "r"); 
    if (filename == NULL) { 
        //input file given in jobfile does not exist or is unable 
        //to be opened for reading. 
        free(line); 
        free_command(com); 
        fclose(jobfile); 
        fprintf(stderr, "testuqwordiply: unable to open file "); 
        fprintf(stderr, "\"%s\" specified on line %d of \"%s\"\n", jobline[0], 
                counter, fname);
        free(duppedline);
        free(jobline);
        exit(5); 
    }
    fclose(filename); 
}

/* process_emptyfile_err() 
 * -----------------------
 * 
 * This function ensures that the jobfile is not empty, only filled 
 * with blank lines or only filled with lines that begin with a "#". 
 * If a jobfile only containts the things mentionaed above, it will 
 * free all allocated memory and exit with a status of 6. It will also print 
 * this message before exiting: 
 * "testuqwordiply: no jobs found in "filename"". Separated by a new line 
 * separator. 
 *
 * com: pointer to a Commands struct that hold will list of commands (if 
 * valid from jobfile) and num of jobs in the list. 
 * jobfile: File pointer to jobfile (specified on command line) that has been 
 * opened for reading. 
 * fname: File name of the jobfile being processed (specified on 
 * command line).
 * 
 * Returns: void 
 *
 * Error: Exit with an Exit Status 6 if jobfile is empty, only filled with 
 * blank lines or only filled with lines that begin with a "#". 
 *
 */
void process_emptyfile_err(Commands* com, FILE* jobfile, char* fname) { 
    fclose(jobfile); 
    free_command(com); 
    fprintf(stderr, "testuqwordiply: no jobs found in "); 
    fprintf(stderr, "\"%s\"\n", fname);
    exit(6);
}

/* free_command() 
 * --------------
 *
 * Function will free all memory allocated for the struct and the 
 * list of commands (if valid from jobfile). 
 *
 * com: pointer to a Commands struct that hold will list of commands (if 
 * valid from jobfile) and num of jobs in the list. 
 *
 * Returns: void
 *
 */
void free_command(Commands* com) { 
    for (int index = 0; index < com->numoflines; index++) { 
        free(com->filejobs[index]); 
    }
    free(com->filejobs); //free the array of strings
    free(com);
}

