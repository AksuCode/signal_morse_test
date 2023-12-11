

/* Open file for read. Takes file_name (char *) as a parameter and attempts to open it.
*  If filename is "-" then open standard input else attempt to open the file. If file opening fails
*  return NULL. On success return FILE * of the opened file.
*/
FILE * open_input_file(char * file_name) {

    if (strcmp(file_name, "-") == 0) {     // If File name is "-" then open as standard input
        flogmessagef(log_file,"Input file opened: stdin\n");
        return stdin;

    } else {

        int ifd = open(file_name, O_RDONLY);
        if (ifd < 0) {     // Check that input file exists and it can be opened
            return NULL;
        }
        close(ifd);
        flogmessagef(log_file,"Input file opened: %s\n", file_name);
        return fopen(file_name, "r");

    }

}


/* Open file for write. Takes file_name (char *) as a parameter and attempts to open it.
*  If filename is "-" then open standard output else attempt to open the file. If file doesn't 
*  exist then create the file. Return NULL if opening a file fails else return FILE * of the opened file.
*/
FILE * open_output_file(char * file_name) {

    if (strcmp(file_name, "-") == 0) {     // If File name is "-" then open as standard output
        flogmessagef(log_file,"Output file opened: stdout\n");
        return stdout;

    } else {
        FILE * tmpf = fopen(file_name, "w");

        if (tmpf == NULL) {                // Check that creating outputfile is successfull
            return tmpf;
        }

        flogmessagef(log_file,"Output file opened: %s\n", file_name);
        return tmpf;
    }

}