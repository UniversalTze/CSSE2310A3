# CSSE2310A3

## Test UQWordiply
The goal of this assignment was to demonstrate skills and ability in fundamental process management and communication concepts, and to further develop your C programming skills with a moderately complex program. I created a program called testuqwordiply that creates and manages communicating collections of processes that test a uqwordiply program (from A1) according to a job specification file that lists tests to be run. For various test cases, the program will run both a test version of uqwordiply and A1 one solution (available as demo-uqwordiply) and compare their standard outputs, standard errors and exit statuses and report the results. I was provided with must use program named uqcmp that compares the input arriving on two file descriptors. The assignment will also test your ability to code to a programming style guide and to use a revision control system appropriately.

## Comments 
This assigment was developed in a linux environment. Vim was the main application used to write code and terminal was used to navigate around files. 
To access the linux environment, a SSH or "Secure Shell" was used. For version control, SVN was used. 

## Marks 
- 60.1/65 (Autograder Score)
- Future improvements: Make sure that file descriptors used (3 and 4) are not open. If currently open, close them and then create a new file descriptor using *open()*. 

## Context on what program does
Notes: File can only be ran in UQ Linux environment through SSH as the Makefile links to libraries provided in that environment. 

### Parameters 

The program (uqwordiply) accepts command line arguments as follows: ./testuqwordiply [--quiet] [--parallel] testprogram jobfile
- --quiet – if present, the output (stdout and stderr) of the uqcmp processes is suppressed (i.e. not shown), otherwise it will output to testuqwordiply’s stdout and stderr respectively. This option does not change testuqwordiply’s output – it will still send output to stdout and stderr as described in this specification. Check example sections. 
- --parallel – if present, the test jobs are run in parallel, otherwise they it occurs sequentially (one after the other).
- testprogram – must be present and is the name of the program being tested against demo-uqwordiply. The name can be a relative or absolute pathname or otherwise (if it doesn’t contain a slash ‘/’) is expected to be found on the user’s PATH. The argument may not begin with --. (If a program’s name begins with -- then the path to the program must be given, e.g. ./--prog.)
- jobfile – must be present (after the program name) and is the name of the file containing details of the tests to be run (referred to in the remainder of this document as the job specification file). More details on the format of this file are given below. The argument may not begin with --. (If a file’s name begins 82 with -- then the path to the file must be given, e.g. ./--jobfile.)

Prior to doing anything else, your testuqwordiply program must check the validity of the command line  arguments. If the program receives an invalid command line then it must print the message: 
"Usage: testuqwordiply [--quiet] [--parallel] testprogram jobfile" 
to standard error and exits with a status of 2. 

Invalid command lines include (but may not be limited to) any of the following: 
- arguments that begin with -- that are not either --quiet or --parallel
- either the --quiet or --parallel argument is specified more than once
- either the --quiet or --parallel argument appears after a mandatory argument
- the program name and/or the job file name are not specified on the command line
- an unexpected argument is present 
- Checking whether the program name and/or the file name are valid is not part of usage checking.
  
#### Job Specification File
If the command line arguments are valid then testuqwordiply reads the job specification file listed on the command line. The whole file is read and checked prior to any test jobs being run.  If testuqwordiply is unable to open the job specification file for reading then it prints the following  message to stderr (with a following newline) and exit with an exit status of 3: 
- *"testuqwordiply: Unable to open job file "filename""*  where filename is replaced by the name of the file (as given on the command line). (The double quotes around
the filename is present in the output message.)

#### File Format
The job specification file provided to testuqwordiply is a text file, with one line per job. The file may contain any number of lines. 
 - If testuqwordiply is able to open the job specification file for reading, it will read and check all of  the test jobs in the file before starting any tests.
 - Lines in the job file beginning with the ‘#’ (hash) character are comments, and are to be ignored by testuqwordiply. Similarly, empty lines (i.e. with no characters before the newline) are to be ignored. All other lines are to be interpreted as job specifications, split over 2 separate fields delimited by the comma (‘,’) character as follows: 
   - input-file-name,[arg1 arg2 ...]
 - The input-file-name field is the name of a file to be used as the standard input to the the test program and to demo-uqwordiply for that test. It may be an absolute pathname or relative to the current directory. It  must not be an empty field. 
- The second field is a space separated list of command line arguments to be provided to the test program and to demo-uqwordiply when the test is run. This field may be empty.
- Arguments that contain spaces may be enclosed by double quotes. Leading spaces and extra separating spaces are ignored.
- Note that individual job specifications are independent – a given test job will always be run in the same way independent of any other test jobs described in the file. The comma character has special meaning in job specification files and will only appear in job lines as a separator.

#### Checking the job
Each job line in the job specification file must be validated sequentially. Lines are counted starting from 1.
Syntax Validation: 
A valid line must contain:
- Exactly one comma.
- At least one character before the comma (non-empty input filename).
  
If a line is not syntactically valid:
- Print the following message to stderr (with a newline):
**"testuqwordiply: syntax error on line linenum of "filename""**
Replace linenum with the line number.
Replace filename with the job specification file name (with double quotes).
Exit with status code 4.
No further lines are checked.

File Opening Validation:
For a syntactically valid line: Try to open the input file specified before the comma.
If the input file cannot be opened:
- Print the following message to stderr (with a newline):
**"testuqwordiply: unable to open file "inputfile" specified on line linenum of "filename""**
Replace inputfile with the name of the file before the comma. Replace linenum with the line number. Replace filename with the job specification file name (with double quotes).
Exit with status code 5.

Empty or Comment-Only Files:
If the file is empty or contains only blank lines and/or comments:
- Print the following message to stderr (with a newline):**"testuqwordiply: no jobs found in "filename""**
Replace filename with the job specification file name (with double quotes).
Exit with status code 6.

#### Example of job fib file
!(Photo)[]

