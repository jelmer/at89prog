AC_INIT(at89prog, 0.9, [jelmer@vernstok.nl])

AC_PROG_CC

PKG_CHECK_MODULES(GTK, gtk+-2.0 glib-2.0 gtkhex, EXTRA_TARGETS="at89prog-gtk", 
				  echo -n)

AC_CHECK_LIB(popt, poptGetOptArg, [], [ AC_MSG_ERROR([popt is required!]) ] )

PKG_CHECK_MODULES(FTDI, libftdi, [
		HAVE_FTDI="1"
		CFLAGS="$CFLAGS -DHAVE_FTDI=1"
		]
		, echo -n)

AC_SUBST(EXTRA_TARGETS)
AC_SUBST(HAVE_FTDI)

AC_CONFIG_FILES([Makefile.settings])
AC_OUTPUT
