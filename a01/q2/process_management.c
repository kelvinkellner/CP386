/*
 -------------------------------------
 File:    process_management.c
 Project: 190668940_190684430_a01_q21
 -------------------------------------
 Author:  Kelvin Kellner & Nishant Tewari 
 ID:      190668940 & 190684430
 Email:   kell8940@mylaurier.ca & tewa4430@mylaurier.ca
 Version  2021-06-05
 -------------------------------------
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "process_management.h"

int main(int argc, char *args[])
{
    if (argc > 1)
    {
        /* PREPARE SHARED MEMORY AREA */

        // open the shared memory segment as if it was a file
        int shm_id = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_id == -1)
        {
            printf("\nError! Failed to create shared memory area: %s\n", strerror(errno));
            exit(1);
        }
        // configure the size of the shared memory segment
        ftruncate(shm_id, SHM_SIZE);
        // map the shared memory segment to the address space of the process
        char *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
        if (shm_ptr == MAP_FAILED)
        {
            printf("\nError! Failed to map shared memory area to process: %s\n", strerror(errno));
            exit(1);
        }

        /* READ FROM FILE AND WRITE TO SHARED MEMORY AREA */
        char *filename = args[1];
        from_file_to_shm(shm_ptr, filename);

        /* RUN COMMANDS FROM SHARED MEMORY AREA */
        run_commands_from_shm(shm_ptr);
    }
    else
    {
        printf("\nError! No file name specified as argument.\n");
        exit(1);
    }
}

void from_file_to_shm(char *shm_ptr, char *filename)
{
    char *ptr = shm_ptr;
    pid_t pid = fork(); // create child process

    if (pid == 0) // child process
    {
        FILE *file = fopen(filename, "r");
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        while ((read = getline(&line, &len, file)) != -1)
        {
            ptr += sprintf(ptr, "%s", line);
        }
        exit(0); // kill child process when finished
    }
    else if (pid > 0) // parent process
    {
        int status;
        wait(&status); // wait for child to finish
        if (WEXITSTATUS(status) == -1)
        {
            perror("\nError! Problem occured while waiting for child to finish in from_file_to_shm function.\n");
            exit(1);
        }
    }
    else
    {
        printf("\nError! Failed to fork in from_file_to_shm function.\n");
        exit(1);
    }
}

void run_commands_from_shm(char *shm_ptr)
{
    char *ptr = shm_ptr;
    char *pipe_name = "/tmp/pipe";
    mkfifo(pipe_name, 0666); // create pipe for writing
    pid_t pid = fork();

    if (pid == 0) // child process
    {
        int pipe_id = open(pipe_name, O_WRONLY);
        char buffer[SHM_SIZE];
        for (int i = 0; i < 64; i += 1)
        {
            // parse each character and concatonate to buffer string
            char next = (char)ptr[i];
            strncat(buffer, &next, 1);
        }
        // break string on line break to get each individual command
        char *buffer_ptr = strtok(buffer, "\r\n");
        write_command_outputs_to_pipe(buffer_ptr, pipe_id);
        close(pipe_id); // kill pipe when finished
    }
    else if (pid > 0) // parent process
    {
        int pipe_id = open(pipe_name, O_RDONLY);
        int status;
        wait(&status); // wait for child to finish
        if (WEXITSTATUS(status) == -1)
        {
            perror("\nError! Problem occured while waiting for child to finish in run_commands_from_shm function.\n");
            exit(1);
        }
        from_pipe_to_file(pipe_id);
        close(pipe_id);
    }
    else
    {
        printf("\nError! Failed to fork in run_commands_from_shm function.\n");
        exit(1);
    }
}

void write_command_outputs_to_pipe(char *buffer_ptr, int pipe_id)
{
    char buffer[SHM_SIZE];
    while (buffer_ptr)
    {
        // Formatting form Discussion post, thanks you :)
        FILE *vf = popen(buffer_ptr, "r");
        char line[1035];
        if (vf)
        {
            // write output of each file to buffer
            char first_line[50];
            sprintf(first_line, "The output of: %s : is\n>>>>>>>>>>>>>>>\n", buffer_ptr);
            strcat(buffer, first_line);
            while (fgets(line, sizeof(line), vf) != NULL)
            {
                strcat(line, "\n");
                strcat(buffer, line);
            }
            strcat(buffer, "<<<<<<<<<<<<<<<");
        }
        else
        {
            printf("\nError! Failed to execute command: '%s'!\n", buffer_ptr);
            exit(1);
        }
        fclose(vf);
        buffer_ptr = strtok(NULL, "\r\n");
    }
    write(pipe_id, buffer, SHM_SIZE + 1); // write buffer contents to pipe
}

void from_pipe_to_file(int pipe_id)
{
    // write contents of pipe to buffer
    char buffer[SHM_SIZE];
    read(pipe_id, buffer, SHM_SIZE);
    FILE *fv = fopen(OUTPUT_FILE_NAME, "w");
    char *ptr = strtok(buffer, "\r\n");
    while (ptr)
    {
        // print each line of buffer to file
        fprintf(fv, "%s\n", ptr);
        ptr = strtok(NULL, "\r\n");
    }
    fclose(fv); // close file when finished
    printf("\nSuccess!\nCheck output file.\n\n");
}
