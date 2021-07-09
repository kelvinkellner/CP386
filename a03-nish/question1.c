/*
 -------------------------------------
 File:    Question1.c
 Project: 190684430_a03_q11
 -------------------------------------
 Author:  Nishant Tewari 
 ID:      190684430
 Email:   tewa4430@mylaurier.ca
 Version  2021-07-10
 -------------------------------------
 */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

// Global Data variables.
int a = 5, b = 7;
// Function that access the global data.
void* inc_dec(void *arg) {
	printf("Read value of 'a' global variable is: %d\n", a);
	printf("Read value of 'b' global variable is: %d\n", b);
	sleep(1);
	a = a + 1;
	b = b - 1;
	printf("Updated value of 'a' variable is: %d\n", a);
	printf("Updated value of 'b' variable is: %d\n", b);
	return 0;
}
int main() {
	// Creating the thread instances.
	pthread_t t1, t2, t3;
	pthread_create(&t1, NULL, inc_dec, NULL);
	pthread_create(&t2, NULL, inc_dec, NULL);
	pthread_create(&t3, NULL, inc_dec, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);

	//Destroying the threads.
	pthread_exit(t1);
	pthread_exit(t2);
	pthread_exit(t3);
	return 0;
}
