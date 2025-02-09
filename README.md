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

## Libraries Used
![Photo5](https://github.com/UniversalTze/CSSE2310A3/blob/main/pictures/Provided_Libs.png)

## Parameters 

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
  
### Job Specification File
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

#### Examples of Valid Job Files
![Photo](https://github.com/UniversalTze/CSSE2310A3/blob/main/pictures/Example1.png)

### Running Test Jobs and Reporting Results
If all of the test jobs in the job specification file are valid then testuqwordiply must run the jobs, either in sequence (one after the other) or in parallel (if the --parallel argument is provided on the command line). The behaviour of these two different modes is described below.

#### Sequential 
If testuqwordiply is running jobs sequentially, then it must perform the following sequence of operations: 158
For each job (in the order specified in the job specification file): 
- Start the job (as described below in Running One Test Job) 
-  Sleep for 2 seconds 
- Send a SIGKILL signal to all processes that make up that job. (It is likely they are already dead, but 162 testuqwordiply does not need to check this prior to attempting to send a signal.) 
- Report the result of the job (as described below in Reporting the Result of One Test Job).
When one job is complete, the next job can be started.

#### Parellel (--parallel)
If testuqwordiply is running jobs in parallel, then it must perform the following sequence of operations:
- Start all of the jobs specified in the job specification file (each individual job should be started as described
below in Running One Test Job)
- Sleep for two seconds
- Send a SIGKILL signal to all processes that make up all jobs. (It is likely they are already dead, but 171 testuqwordiply does not need to check this prior to attempting to send a signal.)
-  Iterate over each job (in the order specified in the job specification file) and report the result of the job (as described below in Reporting the Result of One Test Job).

#### One Test Job
Before running a test job, program will print **"Starting Job T"**, where T is replaced by the job number (1 - N , where N is the number of jobs in the job specification file). 
Four Processes: 
- an instance of the program being tested (as specified on the command line). Standard input for this process must come from the input file specified in the job. Standard output and standard error must be sent to file descriptor 3 of two instances of uqcmp (see description of uqcmp instances below).
- an instance of demo-uqwordiply – which will must be found in the user’s PATH i.e. do not assume a particular location for demo-uqwordiply. Standard input for this process must come from the input file specified in the job. Standard output and standard error must be sent to file descriptor 4 of the two instances of uqcmp. This is the expected output from the test program. 
- an instance of uqcmp which compares standard outputs – which will receive the standard output of the program being tested on file descriptor 3 and the standard output of demo-uqwordiply on file descriptor 4. uqcmp must be started with one command line argument: **“Job T stdout”** where T is replaced by the job number. uqcmp must be found in the user’s PATH – do not assume a particular location for uqcmp. 
- an instance of uqcmp which compares standard errors – which will receive the standard error of the program being tested on file descriptor 3 and the standard error of demo-uqwordiply on file descriptor 4. uqcmp must be started with one command line argument: **“Job T stderr”** where T is replaced by the job number. uqcmp must be found in the user’s PATH – do not assume a particular location for uqcmp.

If the --quiet option is provided on the testuqwordiply command line then the standard output and standard error of the uqcmp instances must be redirected to /dev/null. If --quiet is not specified, then the uqcmp instances must inherit standard output and standard error from testuqwordiply – i.e. send standard 199 output and standard error to wherever testuqwordiply’s standard output and standard error are being sent.

#### Reporting Each Test Result 
If any of the programs were unable to be executed (demo-uqwordiply, uqcmp or the program under test) 205 then testuqwordiply must print and flush the following to standard output (followed by a newline): 
**"Job T: Unable to execute test"** (With the same behaviour for T as stated above). 
The program will report in the given order below. All messages are sent to standard out (flushed at time of printing) and are terminated by a single new line. 
- If the standard outputs of the two programs match (as determined by the exit status of that uqcmp instance), then testuqwordiply must print the following:
**"Job T : Stdout matches"**
- If the standard outputs do not match, then testuqwordiply must print the following: **"Job T : Stdout differs**"
- If the standard errors of the two programs match (as determined by the exit status of that uqcmp instance), then testuqwordiply must print the following: **"Job T: Stderr matches"**
-  If the standard errors do not match, then testuqwordiply must print the following: **"Job T: Stderr differs"**
-  If both processes exit normally with the same exit status, then testuqwordiply must print the following: **"Job T: Exit status matches"**
-  Otherwise it must print: **"Job T: Exit status differs"**

#### Reporting Overall Result
A test passes if the standard output, standard error and exit statuses of the program being tested and  demo-uqwordiply all match each other. For both sequential and parallel modes of operation, when all test jobs have been run and finished, then testuqwordiply must output the following message to stdout (followed by a newline): 
**"testuqwordiply: M out of N tests passed"** 
where M is replaced by the number of tests that passed, and N is replaced by the number of tests that have been run (which may be fewer than the number of the tests in the job specification file if the tests are interrupted).

If all tests that have been run passed then testuqwordiply must exit with exit status 0 (indicating success), otherwise it must exit with exit status 1 (indicating failure).

#### Interrupting the Tests
If testuqwordiply receives a SIGINT (as usually sent by pressing Ctrl-C) then it should complete the test job(s) in progress (including any sleeps), not commence any more test jobs, and report the overall result based on the tests that have been run. 

In practical terms, this will only make a difference in sequential mode – tests that haven’t been started will not be run. In parallel mode, all tests are started immediately, so all tests should be run to completion if a SIGINT is received.

### Examples of test programs and its behaviour
#### Simple One
![Photo2](https://github.com/UniversalTze/CSSE2310A3/blob/main/pictures/Example2.png)
#### Larger One
![Photo3](https://github.com/UniversalTze/CSSE2310A3/blob/main/pictures/Example3.1.png)
![Photo4](https://github.com/UniversalTze/CSSE2310A3/blob/main/pictures/Example3.2.png)




