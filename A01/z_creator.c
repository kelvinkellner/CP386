/*
 -------------------------------------
 File:    z_creator.c
 Project: 190668940_190684430_a01_q01
 -------------------------------------
 Author:  Kelvin Kellner & Nishant Tewari 
 ID:      190668940 & 190684430
 Email:   kell8940@mylaurier.ca & tewa4430@mylaurier.ca
 Version  2021-06-04
 -------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();

    if (pid > 0)
    {
        printf("Parent process.");
        sleep(20);
    }
    else if (pid == 0)
    {
        printf("Child process.");
        exit(0);
    }
    else
    {
        printf("Error!");
        return 1;
    }
    return 0;
}