LIBS = -lpopt 
DEBUG = -g3 #-pg -fprofile-arcs
#DEBUG = 

PROG_OBJ = prog1.o at89ser.o pins.o pins-serial.o pins-serial-raw.o delays.o
DELAYTEST_OBJ = delaytest.o delays.o

all: at89prog
doc: at89prog.pdf

install: all doc
	mkdir -p $(DESTDIR)/usr/bin
	cp at89prog $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/share/man/man1
	cp at89prog.1 $(DESTDIR)/usr/share/man/man1
	mkdir -p $(DESTDIR)/usr/share/doc/at89prog
	cp at89progrc.example TODO COPYING AUTHORS at89prog.pdf $(DESTDIR)/usr/share/doc/at89prog

at89prog.pdf: at89prog.tex
	pdflatex $<
	pdflatex $<

at89prog: $(PROG_OBJ)
	$(CC) $(DEBUG) -Wall -O -o $@ $(PROG_OBJ) $(LIBS)

delaytest: $(DELAYTEST_OBJ)
	$(CC) $(DEBUG) -Wall -O -o $@ $(DELAYTEST_OBJ)

%.o: %.c
	$(CC) $(DEBUG) -Wall -O -c $< -o $@

clean: 
	rm -f at89prog *.o core at89prog.aux at89prog.log at89prog.toc at89prog.pdf
