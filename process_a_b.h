#include "data_send_receive.h"


/* This function is a process that takes pid_t pid of target process as a parameter. The second parameter is an input_file
*  that is transmitted to the pid using signals. Each character in the file is sent individually using morse encoding.
*  If input_file is standard input then this process ends when character ESC ('^[') is given. If input_file is a normal file
*  then this process ends if EOF is reached. When this process is coming to an end ETB ((char)23) value is sent to pid. This process only starts
*  when it catches a SIGUSR1 signal.
*  Returns 0 on success and -1 on failure.
*/
int process_b(pid_t pid, FILE * input_file) {

    flogmessagef(log_file,"Process B: Wait for permission to continue\n");
    if(wait_untill_SIGUSR1() < 0) {     // wait for permission to continue
        perror("Process B: wait_until_SIGUSR1 failed");
        fflush(stderr);
        return -1;
    }
    flogmessagef(log_file,"Process B: SIGUSR1 caught continue process\n");


    int vrt = EOF;
    if (input_file == stdin) {
        flogmessagef(log_file,"Process B: Transmitter set in stdin mode\n");
        printf("Write input below. If output is stdout it gets printed right below input.\nMorse symbols: ABCDEFGHIJLKMNOPQRSTUVWXYZ 1234567890&\'@():,=!.-+\"?/\nPress enter to send input and ESC (^[) + enter to end program.\n\n");
        vrt = (char)27;     // ESC ( ^[ ). Special character to stop program.
    } else {
        flogmessagef(log_file,"Process B: Transmitter set in input file mode\n");
    }


    flogmessagef(log_file,"Process B: Begin transmission\n");
    char new_char = 0;
    while(new_char != vrt) {

        new_char = toupper(fgetc(input_file));      // Read one character from file
        if (new_char == EOF || new_char == '\n' || new_char == (char)27) {
            sleep(0.01);
            continue;
        }

        int retval = send_symbol(new_char, getppid());
        if (retval == -1) {
            // printf("\nSymbol (%c) was not morse encodable\n", new_char);
        }

    }


    flogmessagef(log_file,"Process B: Transmission ended\n");
    int retval = -1;
    while (retval != 0) {
        union sigval val;
        val.sival_int = 23;
        retval = sigqueue(pid, SIGRTMIN, val);     // Send ETB ((char)23) to notifyi end of transmission.
    }
    flogmessagef(log_file,"Process B: transmission ended ETB ((char)23) sent to A process\n");


    return 0;
}


/* This function is a process that takes pid_t pid of target process as a parameter. The second parameter is an output_file
*  that is written into with received signals that have been decoded. This process creates a new fifo file and a new child process.
*  The child process reads from the fifo file using received_symbol function. If a valid character has been aquired then that is 
*  written into the output_file. The parent process calls receiver function and sends the SIGUSR1 signal the pid process which
*  tells that process to continue. When the ETB ((char)23) value is received then Data Saver process proceeds to die.
*  Before Data Saver process dies it sends SIGUSR1 signal to parent process which gives it permission to die.
*  Finally in parent process output_fifo is closed and unlinked before ending.
*  Returns 0 on success and -1 on failure.
*/
int process_a(pid_t pid, FILE * output_file) {

    const char *FIFO = "./receive_fifo";
    if(mkfifo(FIFO, 0666) < 0) {
        if (errno != EEXIST) {
            perror("Process A: Making fifo file failed");
            fflush(stderr);
            return -1;
        }
    }
    flogmessagef(log_file,"Process A: FIFO pipe created\n");


    pid_t pid2 = fork();
    if (pid2 == 0) {    // Child
        flogmessagef(log_file,"Process A child Data Saver created PID: %i\n", getpid());


        FILE * input_fifo = fopen("./receive_fifo", "r");
        if (input_fifo == NULL) {
            perror("Process A Data Saver: Failed to open fifo");
            fflush(stderr);
            return -1;
        }
        flogmessagef(log_file,"Process A Data Saver: FIFO pipe open\n");
        

        flogmessagef(log_file,"Process A Data Saver: Start decoding signals from FIFO\n");
        while (1) {
            char retval = received_symbol(input_fifo);
            if (retval == '\0') {       // If retval == \0 then signal sequence was not morse decodable or EOF reached. We don't save it.
                printf("\nA signal sequence was not morse decodable or EOF reached\n");
            } else if (retval == (char)23) {
                flogmessagef(log_file,"Process A Data Saver: ETB received permission to proceed to stop process\n");
                break;
            } else {
                fputc(retval, output_file);
                fflush(output_file);
            }

        }


        flogmessagef(log_file,"Process A Data Saver: tell parent Signal Receiver to die (SIGUSR1)\n");
        int retval_usr1 = -1;
        while (retval_usr1 != 0) {
            retval_usr1 = kill(getppid(), SIGUSR1);     // Tell parent to die.
        }


    } else if (pid2 > 0){   // Parent
        flogmessagef(log_file,"Process A parent Signal Receiver created PID: %i\n", getpid());


        FILE * output_fifo = fopen("./receive_fifo", "w");      // Open FIFO
        if (output_fifo == NULL) {
            perror("Process A Signal Receiver: Failed to open fifo");
            fflush(stderr);
            return -1;
        }
        flogmessagef(log_file,"Process A Signal Receiver: FIFO pipe open\n");


        if(receiver(output_fifo) < 0) {      // Create signal Receiver
            perror("Process A Signal Receiver: receiver failed\n");
            fflush(stderr);
            return -1;
        }
        flogmessagef(log_file,"Process A Signal Receiver: signal receiver created\n");


        sleep(0.2);     // Delay so that B has probably set up its signal handler. 
        int retval_usr1 = -1;
        while (retval_usr1 != 0) {
            retval_usr1 = kill(pid, SIGUSR1);     // Tell b process that it can continue.
        }
        flogmessagef(log_file,"Process A Signal Receiver: permission to continue (SIGUSR1) sent to B\n");


        flogmessagef(log_file,"Process A Signal Receiver: wait for permission (SIGUSR1) to die from Data Saver\n");
        if(wait_untill_SIGUSR1() < 0) {     // wait for permission to die
            perror("Process A Signal Receiver: wait_until_SIGUSR1 failed");
            fflush(stderr);
            return -1;
        }
        flogmessagef(log_file,"Process A Signal Receiver: permission to end process\n");
        

        if(fclose(output_fifo) != 0) {    // Close fifo.
            perror("Process A Signal Receiver: Closing fifo file failed");
            fflush(stderr);
            return -1;
        }
        if(unlink(FIFO) < 0) {           // Delete fifo file.
            perror("Process A Signal Receiver: Unlinking fifo file failed");
            fflush(stderr);
            return -1;
        }
        flogmessagef(log_file,"Process A Signal Receiver: FIFO closed and unlinked\n");


    } else {
        perror("Process A: Fork failed");
        fflush(stderr);
        return -1;
    }

    return 0;
}
