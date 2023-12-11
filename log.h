

// Create/open log file with the name file_name.
// Returns FILE * pointer on success. Returns NULL if failure occurs.
FILE * open_log_file(char * file_name) {
    return fopen(file_name, "w");
}


// Writes formatted text to file with time stamp. Used for log file.
// Returns 0 on success and -1 on failure.
int flogmessagef(FILE * log_f, char * format, ...) {

    // Message:
    va_list ap;
    va_start(ap, format);

    char msg[200];
    if(vsprintf(msg, format, ap) < 0) return -1;
    va_end(ap);


    // Time stamp:
    struct timespec time_atm;
    if(clock_gettime(CLOCK_REALTIME, &time_atm) < 0) return -1;

    time_t c_time = time_atm.tv_sec;

    struct tm * l_time = localtime(&c_time);

    char time_stamp[50];
    if(sprintf(time_stamp, "[%i:%i:%i ns:%li]: ", l_time->tm_hour, l_time->tm_min, l_time->tm_sec, time_atm.tv_nsec) < 0) return -1;


    // Log:
    char * log_msg = strcat(time_stamp, msg);
    fwrite(log_msg, sizeof(char), strlen(log_msg), log_f);
    fflush(log_f);


    return 0;
}