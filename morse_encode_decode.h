

// Morse symbol count
#define MORSE_S_C 52

// Copied straigh from test bench.
const char *morse_asciiplus_charset = "ABCDEFGHIJLKMNOPQRSTUVWXYZ1234567890&'@():,=!.-+\"?/ ";

// Corresponding signal "." = short and "-" = long. At pos [i][:] index i 
// corresponds to the symbol at place i in morse_asciiplus_charset.
const char * morse_signal[] = {

    ".-",
    "-...",
    "-.-.",
    "-..",
    ".",
    "..-.",
    "--.",
    "....",
    "..",
    ".---",
    "-.-",
    ".-..",
    "--",
    "-.",
    "---",
    ".--.",
    "--.-",
    ".-.",
    "...",
    "-",
    "..-",
    "...-",
    ".--",
    "-..-",
    "-.--",
    "--..",
    ".----",
    "..---",
    "...--",
    "....-",
    ".....",
    "-....",
    "--...",
    "---..",
    "----.",
    "-----",
    ".-...",
    ".----.",
    ".--.-.",
    "-.--.",
    "-.--.-",
    "---...",
    "--..--",
    "-...-",
    "-.-.--",
    ".-.-.-",
    "-....-",
    ".-.-.",
    ".-..-.",
    "..--..",
    "-..-.",
    "........"

};


// Takes char array of '.' and '-' as input.
// Returns corresponding morse symbol (char) for given signal on success.
// NUL ('\0') value on failure.
char signal_to_symbol(const char * signal) {

    // Find match
    int i = 0;
    while (i < MORSE_S_C) {

        if (strcmp(morse_signal[i], signal) == 0) {
            return morse_asciiplus_charset[i];
        }

        i += 1;
    }

    return '\0';
}


// Takes morse encodable symbol (char) as input.
// Returns corresponding signal (char array) for given symbol on success.
// On failure returns char array containing only NUL ('\0').
const char * symbol_to_signal(const char symbol) {

    // Find match
    int i = 0;
    while (i < MORSE_S_C) {
        if (morse_asciiplus_charset[i] == symbol) {
            return morse_signal[i];
        }

        i += 1;
    }

    return "\0";
}