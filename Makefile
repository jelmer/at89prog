LIBS = -lpopt 
# DEBUG = -g3 -pg -fprofile-arcs
DEBUG = 

BINS = at89prog

PROG_OBJ = prog1.o at89ser.o

all: $(BINS)

at89prog: $(PROG_OBJ)
	$(CC) $(DEBUG) -Wall -O -o $@ $(PROG_OBJ) $(LIBS)

%.o: %.c
	$(CC) $(DEBUG) -Wall -O -c $< -o $@

clean: 
	rm -f $(BINS) *.o core
