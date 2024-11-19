all: testuqwordiply
CC=gcc 
CFLAGS=-Wall -pedantic -std=gnu99 -Werror
CLINKS=-I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a3 

#Will need to add commands.c and commands.h to this later on
testuqwordiply: testuqwordiply.c commands.c commands.h process.c process.h 
	$(CC) $(CFLAGS) $(CLINKS) $^ -o $@
	# $@ = name of target, $< = name of first prereq
	# line above same as
	# gcc -Wall -pedantic -std=gnu99 -o $@ $< 
 

clean: 
	rm -f testuqwordiply
