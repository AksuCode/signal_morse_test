#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>

#include "global_variables.h"

#include "log.h"
#include "open_inout_f.h"
#include "process_a_b.h"


int main(int argc, char * argv[]) {

    // Create/open log file with the name data_transfer.log
    log_file = open_log_file("./data_transfer.log");
    if (log_file == NULL) {
        fprintf(stderr,"Opening log file (%s) failed\n Ending program\n", "data_transfer.log");
        return -1;
    }
    flogmessagef(log_file,"Log file %s opened for write\n", "data_transfer.log");
    flogmessagef(log_file,"Begin the main process PID: %i\n", getpid());

    // Handle command line parameters:
    // Expecting two file names
    char * input_file_name;
    char * output_file_name;

    // Check argument count and get file names
    if (argc == 2) {                // Only input file given
        input_file_name = argv[1];
        flogmessagef(log_file,"Input file name given: %s\n", input_file_name);

    } else if (argc == 3) {       // Both files given
        input_file_name = argv[1];
        output_file_name = argv[2];
        flogmessagef(log_file,"Input file name given: %s\n", input_file_name);
        flogmessagef(log_file,"Output file name given: %s\n", output_file_name);

    } else {
        fprintf(stderr,"Usage: %s [input|-] [output|-]\n",argv[0]);
        return -1;
    }


    // Input file:
    FILE * input_file = open_input_file(input_file_name);
    if (input_file == NULL) {
        fprintf(stderr,"Opening input file (%s) failed\n", input_file_name);
        return -1;
    }

    // Output file:
    FILE * output_file;
    if (argc != 3) {
        output_file = stdout;
    } else {
        output_file = open_output_file(output_file_name);
    }
    if (output_file == NULL) {
        fprintf(stderr,"Creating output file (%s) failed\n", output_file_name);
        return -1;
    }


    // Create parent process A and child process B
    pid_t pid;
    pid = fork();
    
    if (pid < 0) {
		perror("Fork failed");
		return -1;
	}

	if (pid == 0) {     // Child
        pid_t p_pid = getppid();
        
        flogmessagef(log_file,"Child process B created PID: %i\n", getpid());
        if(process_b(p_pid, input_file) < 0) return -1;

	} else {        // Parent
        flogmessagef(log_file,"Parent process A created PID: %i\n", getpid());
        if(process_a(pid, output_file) < 0) return -1;
        
	}


    flogmessagef(log_file,"Process %i terminated normally.\n", getpid());
    return 0;
}