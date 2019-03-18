# # Makefile for TCP setting up
# # Group 16 


CC = gcc 
CFLAGS = -Wall -pedantic -std=c11 -ggdb -pthread
# to add tracefiles, add filenames or other macros separated by commas,
# e.g. BASE_TRACEFILES,COALESCE_TRACEFILES,my_test_trace.rep
#TRACEFILES = BASE_TRACEFILES,COALESCE_TRACEFILES,REALLOC_TRACEFILES


#OBJS = networking.c
EXECS = Avatars
 
all: $(EXECS)


Avatars: Avatars.c 
	$(CC) $(CFLAGS) $^ -o Avatars

clean:
	rm -f $(EXECS)
