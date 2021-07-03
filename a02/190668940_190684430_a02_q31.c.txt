/*
 -------------------------------------
 File:    Question3.c
 Project: 190668940_190684430_a02_q31
 -------------------------------------
 Author:  Kelvin Kellner & Nishant Tewari
 ID:      190668940 & 190684430
 Email:   kell8940@mylaurier.ca & tewa4430@mylaurier.ca
 Version  2021-06-19
 -------------------------------------
 */
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>

void logStart(char *tID);  //function to log that a new thread is started
void logFinish(char *tID); //function to log that a thread has finished its time

void startClock();	   //function to start program clock
long getCurrentTime(); //function to check current time since clock was started
time_t programClock;   //the global timer/clock for the program

typedef struct thread //represents a single thread
{
	char tid[4];	//id of the thread as read from file
	int start_time; //time stamp for when thread should be started
	int lifetime;	//number of seconds until thread is terminated
	bool started;
} Thread;

pthread_t new_pthread(void *func, void *param); //aux function for creating new threads

void *threadRun(void *t);						//the thread function, the code executed by each thread
int readFile(char *fileName, Thread **threads); //function to read the file content and build array of threads

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}

	/* read from file to populate threads */
	Thread *threads;							 // create collection of Threads
	int n_threads = readFile(argv[1], &threads); // populate threads and get n_threads as return value

	pthread_t tids[n_threads]; // store real tids from threads

	/* start the program clock */
	startClock();

	/* get ready to start processes */
	bool all_started = false;
	int i;
	long current_time;
	/* loop until all processes are started */
	while (!all_started)
	{
		all_started = true;
		current_time = getCurrentTime();
		for (i = 0; i < n_threads; i++)
		{
			if (!threads[i].started && current_time == threads[i].start_time)
			{
				threads[i].started = true;
				tids[i] = new_pthread(threadRun, &threads[i]);
			}
			all_started = all_started && threads[i].started;
		}
	}

	/* join processes when all threads are finished */
	for (i = 0; i < n_threads; i++)
		pthread_join(tids[i], NULL);

	return 0;
}

int readFile(char *fileName, Thread **threads) //use this method in a suitable way to read file
{
	FILE *in = fopen(fileName, "r");
	if (!in)
	{
		printf("Child A: Error in opening input file...exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(in), &st);
	char *fileContent = (char *)malloc(((int)st.st_size + 1) * sizeof(char));
	fileContent[0] = '\0';
	while (!feof(in))
	{
		char line[100];
		if (fgets(line, 100, in) != NULL)
		{
			strncat(fileContent, line, strlen(line));
		}
	}
	fclose(in);

	char *command = NULL;
	int threadCount = 0;
	char *fileCopy = (char *)malloc((strlen(fileContent) + 1) * sizeof(char));
	strcpy(fileCopy, fileContent);
	command = strtok(fileCopy, "\r\n");
	while (command != NULL)
	{
		threadCount++;
		command = strtok(NULL, "\r\n");
	}
	*threads = (Thread *)malloc(sizeof(Thread) * threadCount);

	char *lines[threadCount];
	command = NULL;
	int i = 0;
	command = strtok(fileContent, "\r\n");
	while (command != NULL)
	{
		lines[i] = malloc(sizeof(command) * sizeof(char));
		strcpy(lines[i], command);
		i++;
		command = strtok(NULL, "\r\n");
	}

	Thread *threads_arr = *threads;
	for (int k = 0; k < threadCount; k++)
	{
		char *token = NULL;
		Thread thread;
		/* read tid from token */
		token = strtok(lines[k], ";");
		strcpy(thread.tid, token);
		/* read start time and lifetime from token */
		token = strtok(NULL, ";");
		thread.start_time = atoi(token);
		token = strtok(NULL, ";");
		thread.lifetime = atoi(token);
		/* default started to false */
		thread.started = false;
		/* ensure that thread is stored in threads collection */
		threads_arr[k] = thread;
	}
	return threadCount;
}

void logStart(char *tID) //invoke this method when you start a thread
{
	printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char *tID) //invoke this method when a thread is over
{
	printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

void *threadRun(void *t) //implement this function in a suitable way
{
	/* log start, sleep for lifetime secs, log finish */
	Thread *thread = t;
	logStart(thread->tid);
	sleep(thread->lifetime);
	logFinish(thread->tid);
}

void startClock() //invoke this method when you start servicing threads
{
	programClock = time(NULL);
}

long getCurrentTime() //invoke this method whenever you want to check how much time units passed
//since you invoked startClock()
{
	time_t now;
	now = time(NULL);
	return now - programClock;
}

pthread_t new_pthread(void *func, void *param)
{
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if (pthread_create(&tid, &attr, func, param) != 0)
	{
		perror("q2. new_pthread: pthread_create() error");
		exit(1);
	}
	return tid;
}