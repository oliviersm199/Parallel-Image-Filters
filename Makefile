CC=gcc
CFLAGS= -fopenmp
SOURCES=lodepng.c

all: rectify.c pool.c convolve.c
	$(CC) $(CFLAGS) rectify.c $(SOURCES) -o rectify
	$(CC) $(CFLAGS) pool.c $(SOURCES) -o pool
	$(CC) $(CFLAGS) convolve.c $(SOURCES) -o convolve
	$(CC) $(CFLAGS) test_equality.c $(SOURCES) -o test_equality -lm
