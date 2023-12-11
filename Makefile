PROGRAM=data_transfer

CFLAGS=-g -Wall -pedantic

all: ${PROGRAM}

${PROGRAM}:

.PHONY: clean
clean:
	rm -rf *.o *~ ${PROGRAM}
	rm -rf *.log
