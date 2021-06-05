/*
 -------------------------------------
 File:    process_management.c
 Project: 190668940_190684430_a01_q01
 -------------------------------------
 Author:  Kelvin Kellner & Nishant Tewari 
 ID:      190668940 & 190684430
 Email:   kell8940@mylaurier.ca & tewa4430@mylaurier.ca
 Version  2021-06-05
 -------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include "process_management.h"

int create_shared_memory(int filename, int size)
{
    // From lecture example :)
    int shm_fd;     // file descriptor, from shm_open()
    char *shm_base; // base address, from mmap()
    char *ptr;      // shm_base is fixed, ptr is movable

    /* create the shared memory segment as if it was a file */
    shm_fd = shm_open(filename, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        printf("prod: Shared memory failed: %s\n", strerror(errno));
        exit(1);
    }

    /* configure the size of the shared memory segment */
    ftruncate(shm_fd, size);

    /* map the shared memory segment to the address space of the process */
    shm_base = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
        printf("prod: Map failed: %s\n", strerror(errno));
        // close and shm_unlink?
        exit(1);
    }
}

int main()
{
    const char *name = "/shm-example"; // file name
    const int SIZE = 4096;             // file size

    // create shared memory space
    int shm_fd = create_shared_memory(name, SIZE);
}