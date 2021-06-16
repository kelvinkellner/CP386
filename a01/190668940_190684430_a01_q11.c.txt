/*
 -------------------------------------
 File:    z_creator.c
 Project: 190668940_190684430_a01_q11
 -------------------------------------
 Author:  Kelvin Kellner & Nishant Tewari 
 ID:      190668940 & 190684430
 Email:   kell8940@mylaurier.ca & tewa4430@mylaurier.ca
 Version  2021-06-05
 -------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();

    // Parent
    if (pid > 0)
    {
        printf("In Parent Process!\n");
        sleep(50);
    }
    // Child
    else if (pid == 0)
    {
        printf("In Child Process!\n");
        exit(0);
    }
    // Invalid pid
    else
    {
        printf("Error!\n");
        return 1;
    }
    return 0;
}