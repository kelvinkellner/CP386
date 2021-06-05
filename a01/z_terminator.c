/*
 -------------------------------------
 File:    z_terminator.c
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
    // Run zombie creator
    system("./z_creator &");

    // Print process before
    printf("\nBefore creating zombie process:\n\n");
    system("ps -l");
    sleep(2);

    printf("\nWith zombie process:\n\n");
    system("ps -l");
    sleep(2);

    // Identify parents of zombie processes
    system("ps -l|grep -w Z|tr -s ' '|cut -d ' ' -f 5");
    sleep(2);

    // Kill parents
    system("kill -9 $(ps -l|grep -w Z|tr -s ' '|cut -d ' ' -f 5)");
    sleep(2);

    // After killing parents of zombies
    printf("\nAfter killing parent of zombie process:\n\n");
    system("ps -l");

    return 0;
}