AC_INIT(Leafpad, 0.8.5, tarot@sdf.lonestar.org)
AM_INIT_AUTOMAKE($PACKAGE_TARNAME, $PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

AC_PROG_INTLTOOL(, no-xml)	# require >= 0.31 for autoreconf
AC_PROG_CC
if test "$GCC" = "yes"; then
	CFLAGS="$CFLAGS -Wall"
fi

AM_PATH_GTK_2_0

AC_ARG_ENABLE(chooser,
	AC_HELP_STRING([--enable-chooser], [use GtkFileChooser instead of GtkFileSelector]),
	AC_DEFINE(ENABLE_CHOOSER, 1, [Define if GtkFileChooser is enabled.])
)
AC_ARG_ENABLE(gnomeprint,
	AC_HELP_STRING([--enable-gnomeprint], [use GnomePrint for printing (experimental)]),
	AC_DEFINE(ENABLE_GNOMEPRINT, 1, [Define if GnomePrint is enabled.])
)
AC_ARG_ENABLE(print,
	AC_HELP_STRING([--disable-print], [do not include print feature]),
	AC_DEFINE(DISABLE_PRINT, 1, [Define if printing feature is disabled.])
)
if test "$enable_print" != "no"; then
	if test "$enable_gnomeprint" = "yes"; then
		PKG_CHECK_MODULES(GNOMEPRINT, libgnomeprintui-2.2)
		AC_SUBST(GNOMEPRINT_CFLAGS)
		AC_SUBST(GNOMEPRINT_LIBS)
	fi
fi

ALL_LINGUAS="bg ca cs de el es fr he hu it ja lt pl pt ru sk sv ta tr vi zh_CN zh_TW"
GETTEXT_PACKAGE="$PACKAGE"
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in])
AC_OUTPUT
