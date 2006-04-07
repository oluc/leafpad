AC_INIT(Leafpad, 0.8.8, tarot@sdf.lonestar.org)
AM_INIT_AUTOMAKE($PACKAGE_TARNAME, $PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

AC_PROG_INTLTOOL(, no-xml)	# require >= 0.31 for autoreconf
AC_PROG_CC
if test "$GCC" = "yes"; then
	CFLAGS="$CFLAGS -Wall"
fi

AM_PATH_GTK_2_0

AC_ARG_ENABLE(chooser,
	AC_HELP_STRING([--disable-chooser], [force to use GtkFileSelector]))
if test "$enable_chooser" != "no"; then
    if pkg-config --atleast-version=2.4.0 gtk+-2.0; then
		AC_DEFINE(ENABLE_CHOOSER, 1, [Define if GtkFileChooser is enabled.])
	fi
fi

AC_ARG_ENABLE(print,
	AC_HELP_STRING([--disable-print], [force to disable print feature]))
if test "$enable_print" != "no"; then
	PKG_CHECK_MODULES(GNOMEPRINT, libgnomeprintui-2.2, [
		AC_SUBST(GNOMEPRINT_CFLAGS)
		AC_SUBST(GNOMEPRINT_LIBS)
		AC_DEFINE(ENABLE_PRINT, 1, [Define if print feature is enabled.])
	], echo "no")
fi

ALL_LINGUAS="bg ca cs de el es fr he hu it ja lt pl pt ru sk sv ta tr vi zh_CN zh_TW"
GETTEXT_PACKAGE="$PACKAGE"
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in])
AC_OUTPUT
