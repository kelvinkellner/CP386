/*
 -------------------------------------
 File:    Question2.c
 Project: 190684430_a03_q11
 -------------------------------------
 Author:  Nishant Tewari 
 ID:      190684430
 Email:   tewa4430@mylaurier.ca
 Version  2021-07-10
 -------------------------------------
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h> 

sem_t running;
sem_t even;
sem_t odd;

int thread_one = 1;     	// using Binary digits 1 and 0 set as True and False 
int thread_ready = 0;

void logStart(char *tID); 	//function to log that a new thread is started
void logFinish(char *tID);	//function to log that a thread has finished its time

void startClock(); 			//function to start program clock
long getCurrentTime(); 		//function to check current time since clock was started
time_t programClock; 		//the global timer/clock for the program

typedef struct thread{ 								//represents a single thread, you can add more members if required
	char tid[4]; 									//id of the thread as read from file
	unsigned int startTime;
	int state;
	pthread_t handle;
	int retVal;
} Thread;


int threadsLeft(Thread *threads, int threadCount);
int threadToStart(Thread *threads, int threadCount);
void* threadRun(void *t); 							//the thread function, the code eiecuted by each thread
int readFile(char *fileName, Thread **threads); 	//function to read the file content and build array of threads

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Input file name missing...eiiting with error code -1\n");
		return -1;
	}
	//sempahores initialized here
	sem_init(&running, 0, 1);
	sem_init(&even, 0, 0);
	sem_init(&odd, 0, 0);

	Thread *threads = NULL;
	int threadCount = readFile(argv[1], &threads);

	startClock();
	while (threadsLeft(threads, threadCount) > 0) {
		int i = 0;
		while ((i = threadToStart(threads, threadCount)) > -1) {
			if (thread_one){
				if (threads[i].tid[2] % 2)
					sem_post(&odd);
				else
					sem_post(&even);
				thread_one = 0;
			}
			threads[i].state = 1;
			threads[i].retVal = pthread_create(&(threads[i].handle), NULL,
					threadRun, &threads[i]);
		}
			if (!thread_ready){
			int checkAllReady = 1;
			for (int i = 0; i < threadCount; i++){ 
			
				if (threads[i].state == 0)
					checkAllReady = 0;
			}
			if (checkAllReady) // Updating the thread_ready
				thread_ready = 1; 
			}
	}
	//After it is finished, destroy the sempahores!
	sem_destroy(&running);
	sem_destroy(&even);
	sem_destroy(&odd);

	return 0;
}

int readFile(char *fileName, Thread **threads){ //***do not modify this method***

	FILE *in = fopen(fileName, "r");
	if (!in) {
		printf(
				"Child A: Error in opening input file...eiiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(in), &st);
	char *fileContent = (char*) malloc(((int) st.st_size + 1) * sizeof(char));
	fileContent[0] = '\0';
	while (!feof(in)) {
		char line[100];
		if (fgets(line, 100, in) != NULL) {
			strncat(fileContent, line, strlen(line));
		}
	}
	fclose(in);

	char *command = NULL;
	int threadCount = 0;
	char *fileCopy = (char*) malloc((strlen(fileContent) + 1) * sizeof(char));
	strcpy(fileCopy, fileContent);
	command = strtok(fileCopy, "\r\n");
	while (command != NULL) {
		threadCount++;
		command = strtok(NULL, "\r\n");
	}
	*threads = (Thread*) malloc(sizeof(Thread) * threadCount);

	char *lines[threadCount];
	command = NULL;
	int i = 0;
	command = strtok(fileContent, "\r\n");
	while (command != NULL) {
		lines[i] = malloc(sizeof(command) * sizeof(char));
		strcpy(lines[i], command);
		i++;
		command = strtok(NULL, "\r\n");
	}

	for (int k = 0; k < threadCount; k++) {
		char *token = NULL;
		int j = 0;
		token = strtok(lines[k], ";");
		while (token != NULL) {
		//if you have eitended the Thread struct then here
		//you can do initialization of those additional members
		//or any other action on the Thread members
			(*threads)[k].state = 0;
			if (j == 0)
				strcpy((*threads)[k].tid, token);
			if (j == 1)
				(*threads)[k].startTime = atoi(token);
			j++;
			token = strtok(NULL, ";");
		}
	}
	return threadCount;
}

void logStart(char *tID) {
	printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char *tID) {
	printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

int threadsLeft(Thread *threads, int threadCount) {
	int remainingThreads = 0;
	for (int k = 0; k < threadCount; k++) {
		if (threads[k].state > -1)
			remainingThreads++;
	}
	return remainingThreads;
}

int threadToStart(Thread *threads, int threadCount) {
	for (int k = 0; k < threadCount; k++) {

		if (threads[k].state == 0 && threads[k].startTime == getCurrentTime())
			return k;
	}
	return -1;
}

void* threadRun(void *arg){ 

	Thread *t = (Thread *)arg;
	logStart(t ->tid);

	//If threads are NOT ready, wait for the odd or even semaphores!
	if (!thread_ready){
		if (t->tid[2] % 2)
			sem_wait(&odd);
		else
			sem_wait(&even);
		sem_wait(&running);
	}

	//critical section starts here
	printf("[%ld] Thread %s is in its critical section\n", getCurrentTime(),((Thread*) t)->tid);
	//critical section ends here

	if (thread_ready){
		//if all the threads are ready then unlock the semaphores 
		int val;
		sem_getvalue(&even, &val);
		if (val == 0)
			sem_post(&even);
		sem_getvalue(&odd, &val);
		if (val == 0)
			sem_post(&odd);
		sem_post(&running);	
		
	}else{
		//ELSE If the threads are NOT ready, unlock the opposite semaphore
		if (t->tid[2] % 2)
			sem_post(&even);
		else
			sem_post(&odd);
		sem_post(&running);
	}
	//Thread here is finished and exit
	logFinish(t->tid); 
	t->state = -1;
	pthread_exit(0);

}
void startClock() {
	programClock = time(NULL);
}

long getCurrentTime(){
//invoke this method whenever you want check how much time units passed since you invoked startClock()
	time_t now;
	now = time(NULL);
	return now - programClock;
}