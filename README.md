# CSSE2310A3

## UQWordiply
Allows users to play the Wordiply game. It prompts the user to enter five guesses (words) that contain a given 3-4 letter combination of letters that are known as the "*starter word*". The goal is to enter valid words (based on a dictionary provided to us). After entering your score is reported as the sum of the lengths of the words entered, and the longest possible word(s) from the dictionary are also reported. The user can choose their own starter word, or the game will choose a random one for you.

## Comments 
This assigment was developed in a linux environment. Vim was the main application used to write code and terminal was used to navigate around files. 
To access the linux environment, a SSH or "Secure Shell" was used. 

## Marks 
- 62.25/65 (Autograder Score)

## Instructions to run the game
Notes: File can only be ran in UQ Linux environment through SSH.

### Parameters 

The program (uqwordiply) accepts command line arguments as follows: ./uqwordiply [--start starter-word | --len length ] [--dictionary filename ]. 

The square brackets ([]) indicate optional arguments. The pipe symbol (|) indicates a choice – i.e. in this case either the --start or the --len option argument (with associated value) can be given but not both. The italics indicate placeholders for user-supplied arguments. 
Some examples of how the program might be run include the following: 
- ./uqwordiply
- ./uqwordiply --len 3
- ./uqwordiply --start top
- ./uqwordiply --dictionary mywords --start row
- ./uqwordiply --start row --dictionary ./mywords

Options can be in any order, as shown in the last two examples. The meaning of the arguments is as follows:
 --start – if specified, this option argument is followed by a 3 or 4 letter starter word (combination of letters, in uppercase, lowercase or mixed-case) to be used when playing the game. (This argument can only be specified if the --len argument is not given.)
- --len – if specified, this option argument is followed by the number 3 or 4, to indicate the length of the 73 random starter word to be chosen. (This argument can only be specified if --start is not given.) 74
- --dictionary – if specified, this option argument is followed by the name of a file that is to be used as 75 the dictionary of valid words.

### Command Line validy check.
If any of these conditions are true, program will exit with a status of 1: 
  - A valid option argument is given (i.e. --start, --len or --dictionary) but it is not followed by an associated value argument.
  - Both the --start and --len option arguments (with associated values) are specified on the command line. 
  - The value associated with the --len argument is neither 3 nor 4. 
  - Any of the option arguments are listed more than once (with associated values). Note that the command 93 line arguments --start --start would not be an invalid command line – this would be an invalid starter 94 word error – see below. 95
  - An unexpected argument is present.
