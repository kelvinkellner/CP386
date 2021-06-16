/*
 -------------------------------------
 File:    process_management.h
 Project: 190668940_190684430_a01_q21
 -------------------------------------
 Author:  Kelvin Kellner & Nishant Tewari 
 ID:      190668940 & 190684430
 Email:   kell8940@mylaurier.ca & tewa4430@mylaurier.ca
 Version  2021-06-05
 -------------------------------------
 */
// CONSTANTS
const int SHM_SIZE = 4096;                   // Shared memory area size in bits
const char *SHM_NAME = "/shm";               // Shared memory area name
const char *OUTPUT_FILE_NAME = "output.txt"; // Output file name

/* Main function to run program :) Run from Linux system and pass file name as first arg */
int main(int argc, char *argv[]);

/* Reads line separated list of commands from input file, writes commands to shared memory area */
void from_file_to_shm(char *shm_ptr, char *filename);

/* Executes commands from shared memory area */
void run_commands_from_shm(char *shm_ptr);

/* Helper function: Writes terminal outputs from each command to temporary data pipe */
void write_command_outputs_to_pipe(char *buffer_ptr, int pipe_id);

/* Writes information from data pipe to output file */
void from_pipe_to_file(int pipe_id);
