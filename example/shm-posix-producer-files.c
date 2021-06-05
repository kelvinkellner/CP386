/**
 * Simple program demonstrating shared memory in POSIX systems.
 *
 * This is the producer process that writes to the shared memory region.
 *
 * Figure 3.17
 *
 * @author Silberschatz, Galvin, and Gagne
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 *
 * modifications by dheller@cse.psu.edu, 31 Jan. 2014
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
#include <sys/wait.h>
#include <errno.h>

void from_file_to_shared_memory(char *shm_name, int shm_size, char *filename, int max_line);
void execute_from_shared_memory(char *shm_name, int shm_size, int max_line);
void write_command_output_to_pipe(int shm_size, char *pointer, int pipe_id);
void display(char *prog, char *bytes, int n);

const char *OUTPUT_FILE_NAME = "output.txt";

int main(void)
{
  char *name = "/shm-example";      // memory file name
  int SIZE = 4096;                  // memory file size
  char *filename = "sample_in.txt"; // input file name
  int MAX_LINE = 64;                // input file max line length

  from_file_to_shared_memory(name, SIZE, filename, MAX_LINE);
  execute_from_shared_memory(name, SIZE, MAX_LINE);
}

void execute_from_shared_memory(char *shm_name, int shm_size, int max_line)
{
  int shm_fd;           // file descriptor, from shm_open()
  char *shm_base;       // base address, from mmap()
  char buffer;          // initialize buffer character
  char *ptr = shm_base; // pointer for current character of string

  /* open the shared memory segment as if it was a file */
  shm_fd = shm_open(shm_name, O_RDONLY, 0666);
  if (shm_fd == -1)
  {
    printf("cons: Shared memory failed: %s\n", strerror(errno));
    exit(1);
  }

  /* map the shared memory segment to the address space of the process */
  shm_base = mmap(0, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
  if (shm_base == MAP_FAILED)
  {
    printf("cons: Map failed: %s\n", strerror(errno));
    // close and unlink?
    exit(1);
  }

  /* read from the mapped shared memory segment */
  display("cons", shm_base, 64); // first as bytes, then as a string
  printf("%s", shm_base);

  char *pipe_name = "/process-pipe";
  mkfifo(pipe_name, 0666);

  printf("\n\n\n HERE WE GO:\n");
  pid_t pid = fork();

  if (pid == 0)
  {
    int pipe_id = open(pipe_name, O_WRONLY);
    char result[shm_size];
    for (int i = 0; i < 64; i += 1)
    {
      char buffer = (char)ptr[i];
      strncat(result, &buffer, 1);
    }
    char *result_pointer = strtok(result, "\r\n");
    write_command_output_to_pipe(shm_size, result_pointer, pipe_id);
    close(pipe_id);
  }
  else if (pid > 0)
  {
    int pipe_id = open(pipe_name, O_RDONLY);
    int status;
    wait(&status);
    if (WEXITSTATUS(status) == -1)
    {
      perror("\nexec_commands_from_memory: Error while waiting\n");
      exit(-1);
    }
    char result[shm_size];
    read(pipe_id, result, shm_size);

    FILE *output_file = fopen(OUTPUT_FILE_NAME, "w");
    char *pointer = strtok(result, "\r\n");
    while (pointer)
    {
      fprintf(output_file, "%s\n", pointer);
      pointer = strtok(NULL, "\r\n");
    }

    fclose(output_file);
    close(pipe_id);
  }

  /* remove the mapped shared memory segment from the address space of the process */
  if (munmap(shm_base, shm_size) == -1)
  {
    printf("cons: Unmap failed: %s\n", strerror(errno));
    exit(1);
  }

  /* close the shared memory segment as if it was a file */
  if (close(shm_fd) == -1)
  {
    printf("cons: Close failed: %s\n", strerror(errno));
    exit(1);
  }

  /* remove the shared memory segment from the file system */
  if (shm_unlink(shm_name) == -1)
  {
    printf("cons: Error removing %s: %s\n", shm_name, strerror(errno));
    exit(1);
  }
}

void write_command_output_to_pipe(int shm_size, char *pointer, int pipe_id)
{
  char result[shm_size];
  while (pointer)
  {
    FILE *virtual_file = popen(pointer, "r");
    char line[1035];
    if (virtual_file)
    {
      char first_line[50];
      sprintf(first_line, "The output of: %s : is\n>>>>>>>>>>>>>>>\n", pointer);
      strcat(result, first_line);

      while (fgets(line, sizeof(line), virtual_file) != NULL)
      {
        strcat(line, "\n");
        strcat(result, line);
      }

      strcat(result, "<<<<<<<<<<<<<<<");
    }
    else
    {
      printf("\nexecute_commands: Error while executing '%s'!\n",
             pointer);
      exit(-1);
    }

    fclose(virtual_file);
    pointer = strtok(NULL, "\r\n");
  }

  write(pipe_id, result, shm_size + 1);
}

/* thank you for the good lecture example, came in handy :) */
void from_file_to_shared_memory(char *shm_name, int shm_size, char *filename, int max_line)
{
  pid_t pid = fork();
  if (pid == 0)
  {
    /* initiliaze input file for reading */
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
      perror("Error opening file");
    }
    char line[max_line];

    int shm_fd;     // file descriptor, from shm_open()
    char *shm_base; // base address, from mmap()
    char *ptr;      // shm_base is fixed, ptr is movable

    /* create the shared memory segment as if it was a file */
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
      printf("prod: Shared memory failed: %s\n", strerror(errno));
      exit(1);
    }

    /* configure the size of the shared memory segment */
    ftruncate(shm_fd, shm_size);

    /* map the shared memory segment to the address space of the process */
    shm_base = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_base == MAP_FAILED)
    {
      printf("prod: Map failed: %s\n", strerror(errno));
      // close and shm_unlink?
      exit(1);
    }

    /**
   * Write to the mapped shared memory region.
   *
   * We increment the value of ptr after each write, but we
   * are ignoring the possibility that sprintf() fails.
   */
    //display("prod", shm_base, 64);
    ptr = shm_base;
    while (fgets(line, max_line, fp) != NULL)
    {
      ptr += sprintf(ptr, "%s", line);
    }
    ///display("prod", shm_base, 64);

    /* remove the mapped memory segment from the address space of the process */
    if (munmap(shm_base, shm_size) == -1)
    {
      printf("prod: Unmap failed: %s\n", strerror(errno));
      exit(1);
    }

    /* close the shared memory segment as if it was a file */
    if (close(shm_fd) == -1)
    {
      printf("prod: Close failed: %s\n", strerror(errno));
      exit(1);
    }
  }
  else
  {
    printf("Error while reading from file");
  }
}

void display(char *prog, char *bytes, int n)
{
  printf("display: %s\n", prog);
  for (int i = 0; i < n; i++)
  {
    printf("%02x%c", bytes[i], ((i + 1) % 16) ? ' ' : '\n');
  }
  printf("\n");
}

/* Citations:
1. Lectures
*/
