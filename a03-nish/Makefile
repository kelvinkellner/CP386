#Do not edit the contents of this file.
CC = gcc
CFLAGS = -Werror -Wall -g -std=gnu99
LDFLAGS = -lrt -lpthread
TARGET = Question1 Question2 
OBJFILES = Question1.o Question2.o 
all: $(TARGET)
Question1: Question1.c
	$(CC) $(CFLAGS) -o Question1 Question1.c $(LDFLAGS)
Question2: Question2.c
	$(CC) $(CFLAGS) -o Question2 Question2.c $(LDFLAGS)

runq1: Question1
	./Question1
runq2: Question2
	./Question2 sample3_in.txt
clean:
	rm -f $(OBJFILES) $(TARGET) *~ 