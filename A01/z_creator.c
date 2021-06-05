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
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t child_pid = fork();
    // Parent process
    if (child_pid > 0)
    {
        sleep(10);
    }
    else
    {
        exit(0);
    }
    return 0;
}

// Does not work: ps -l (no Z programs listed)
// Works: ps -ef | grep a.out (says 1 defunct program)
