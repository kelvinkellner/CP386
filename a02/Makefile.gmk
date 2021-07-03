# Do not edit the contents of this file.
CC = gcc
CFLAGS = -Werror -Wall -g -std=gnu99
LDFLAGS = -lrt -lpthread
TARGET = Question1 Question2 Question3
OBJFILES = Question1.o Question2.o Question3.o
all: $(TARGET)
Question1: Question1.c
	$(CC) $(CFLAGS) -o Question1 Question1.c $(LDFLAGS)
Question2: Question2.c
	$(CC) $(CFLAGS) -o Question2 Question2.c $(LDFLAGS)
Question3: Question3.c
	$(CC) $(CFLAGS) -o Question3 Question3.c $(LDFLAGS)

runq1: Question1
	./Question1
runq2: Question2
	./Question2 98 100 102 43 43 54 64 12 43 54 11 23
runq3: Question3
	./Question3 sample2_in.txt
clean:
	rm -f $(OBJFILES) $(TARGET) *~ 