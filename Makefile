LIBS = -lpopt 
DEBUG = -g3 #-pg -fprofile-arcs
GTK_LIBS = `pkg-config --libs gtk+-2.0 glib-2.0`
GTK_CFLAGS = `pkg-config --cflags gtk+-2.0 glib-2.0`
#DEBUG = 

COMMON_OBJ = at89ser.o pins.o pins-serial.o pins-serial-raw.o delays.o pins-parallel.o
PROG_OBJ = prog1.o $(COMMON_OBJ)
GTKPROG_OBJ = at89prog-gtk.o $(COMMON_OBJ)

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

at89prog-gtk: $(GTKPROG_OBJ)
	$(CC) $(DEBUG) -Wall -O -o $@ $(GTKPROG_OBJ) $(LIBS) $(GTK_LIBS)

%.o: %.c
	$(CC) $(GTK_CFLAGS) $(DEBUG) -Wall -O -c $< -o $@

clean: 
	rm -f at89prog *.o core at89prog.aux at89prog.log at89prog.toc at89prog.pdf

ctags: 
	ctags *.c
