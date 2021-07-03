/*
 -------------------------------------
 File:    Question1.c
 Project: 190668940_190684430_a02_q11
 -------------------------------------
 Author:  Kelvin Kellner & Nishant Tewari
 ID:      190668940 & 190684430
 Email:   kell8940@mylaurier.ca & tewa4430@mylaurier.ca
 Version  2021-06-19
 -------------------------------------
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* aux function for printing 5 times with 1 second delay between iterations */
void *func(void *msg);

void main(void)
{
    pthread_t tid;       // thread identifier
    pthread_attr_t attr; // set of thread attributes

    printf("This program would create threads\n");

    /* init with default attributes */
    pthread_attr_init(&attr);

    /* create the thread and call func */
    if (pthread_create(&tid, &attr, func, "I am a Custom Thread Function Created By user.") == 0)
    {
        printf("Custom thread created Successfully\n");

        /* call func from main as well*/
        func("I am the process thread created by compiler By default.");
    }
    /* or print error message if fails */
    else
    {
        perror("pthread_create() error");
        exit(1);
    }

    /* wait for thread to exit, print error message if needed */
    if (pthread_join(tid, NULL) != 0)
    {
        perror("pthread_join() error");
        exit(3);
    };
}

void *func(void *msg)
{
    int i;
    for (i = 0; i < 5; i++)
    {
        printf("%s\n", (char *)msg);
        sleep(1);
    }
}