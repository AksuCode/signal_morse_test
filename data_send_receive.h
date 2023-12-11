#include "morse_encode_decode.h"


// Takes char as input and sends the encoding (symbol_to_signal) 
// of the character to process (pid) as rts signals. Returns 0 on success and returns -1 if given character (new_char) is not morse encodable.
// This function keeps sending signals to pid until every signal has been sent without sigqueue returning -1.
int send_symbol(const char new_char, pid_t pid) {

    const char * signal = symbol_to_signal(new_char);       // Get signal encoding

    if (signal[0] != '\0') {

        union sigval val;

        const char * ptr = signal;
        for (; *ptr != '\0'; ptr++) {   // Get characters from char array until termination character.
            char c = *ptr;
            val.sival_int = (int)c;

            int retval = -1;
            while (retval == - 1) {
                retval = sigqueue(pid, SIGRTMIN, val);   // Send signal with the value in char c to pid process.
            }

        }
        val.sival_int = (int)'*';
        int retval = -1;
        while (retval == - 1) {
            retval = sigqueue(pid, SIGRTMIN, val);   // Send signal with '*' to mark the end of transmission of a symbol.
        }

    } else {
        return -1;
    }

    return 0;
}


// Takes FILE * that is opened in read mode as input.
// Returns the symbol (char) that is decoded from the characters in the file using signal_to_symbol.
// Returns NUL ('\0') on failure and ETB ((char)23) when ETB ((char)23) is received.
char received_symbol(FILE * input_fifo) {

    char morse_encoded[9];

    int i = 0;
    while(i < 9) {  // Take at most 9 characters from the file because no morse code is longer.

        char received_signal = (char)fgetc(input_fifo);

        switch (received_signal) {
            case '*':
                morse_encoded[i] = '\0';
                char symbol = signal_to_symbol(morse_encoded);
                if (symbol != '\0') {
                    return symbol;
                }
                return '\0';
            case EOF:
                return '\0';
            case (char)23:
                return (char)23;
            default:
                morse_encoded[i] = received_signal;
        }

        i += 1;
    }

    return '\0';
}


// Handler function for sa_sigaction. Puts values from signals to the input fifo file.
static void handler(int signum, siginfo_t *info, void *context) {

  switch (info->si_code) {
    case SI_QUEUE:
        fputc((char)info->si_value.sival_int, out_fifo);
        fflush(out_fifo);
        break;
    default:
        break;
  }

}


// Gives global variable (FILE * in_fifo) its value.
// Specifies and initializes the action for SI_QUEUE signals.
// Returns 0 on success and -1 on failure.
int receiver(FILE * output_fifo) {

    out_fifo = output_fifo;

    struct sigaction act;
    if(sigemptyset(&act.sa_mask) < 0) return -1;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;

    if(sigaction(SIGRTMIN, &act, NULL) < 0) return -1;

    return 0;
}


// Empty handler that does nothing.
void empty_handler(int s) {
    return;
}

// This function halts program until SIGUSR1 is caught.
// Returns 0 on success and -1 on failure.
int wait_untill_SIGUSR1() {

    int sig;
    sigset_t sig_set;
    if(sigemptyset(&sig_set) < 0) return -1;
    signal(SIGUSR1, empty_handler);
    if(sigaddset(&sig_set, SIGUSR1) < 0) return -1;
    sigwait(&sig_set, &sig);
    return 0;
}