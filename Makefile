include Makefile.settings

COMMON_OBJ = at89ser.o pins.o pins-serial.o pins-serial-raw.o delays.o pins-parallel.o hexfile.o
FTDI_OBJ = pins-ftdi.o

ifeq ($(HAVE_FTDI),1)
LIBS += $(FTDI_LIBS)
CFLAGS += $(FTDI_CFLAGS)
COMMON_OBJ += $(FTDI_OBJ)
endif 

PROG_OBJ = at89prog.o $(COMMON_OBJ)
GTKPROG_OBJ = at89prog-gtk.o $(COMMON_OBJ)

all: at89prog $(EXTRA_TARGETS)
doc: at89prog.pdf

Makefile.settings: Makefile.settings.in configure
	./configure

configure: configure.ac
	autoreconf

install: all doc
	mkdir -p $(DESTDIR)$(bindir)
	cp at89prog $(DESTDIR)$(bindir)
	mkdir -p $(DESTDIR)$(mandir)/man1
	cp at89prog.1 $(DESTDIR)$(mandir)/man1
	mkdir -p $(DESTDIR)$(mandir)/man5
	cp at89progrc.5 $(DESTDIR)$(mandir)/man5
	mkdir -p $(DESTDIR)$(docdir)
	cp at89progrc.example TODO COPYING AUTHORS at89prog.pdf $(DESTDIR)$(docdir)

at89prog.pdf: at89prog.tex
	pdflatex $<
	pdflatex $<

at89prog: $(PROG_OBJ)
	$(CC) $(DEBUG) -Wall -O -o $@ $(PROG_OBJ) $(LIBS)

at89prog-gtk: $(GTKPROG_OBJ)
	$(CC) $(DEBUG) -Wall -O -o $@ $(GTKPROG_OBJ) $(LIBS) $(GTK_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(DEBUG) -Wall -O -c $< -o $@

clean: 
	rm -f at89prog *.o core at89prog.aux at89prog.log at89prog.toc at89prog.pdf at89prog-gtk

ctags: 
	ctags *.c
