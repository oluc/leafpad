# Process this file with autoconf to produce a configure script.
AC_INIT(Leafpad, 0.7.6, tarot@sdf.lonestar.org)
AM_INIT_AUTOMAKE($PACKAGE_TARNAME, $PACKAGE_VERSION)
AC_CONFIG_SRCDIR([src/leafpad.h])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
CFLAGS="$CFLAGS -Wall"

# Checks for libraries.
AM_PATH_GTK_2_0

# Checks for header files.
#AC_HEADER_STDC
#AC_CHECK_HEADERS([libintl.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST

# Checks for library functions.
#AC_CHECK_FUNCS([setenv strcasecmp strncasecmp strstr])

# gettext
ALL_LINGUAS="de es fr ja ru sv zh_CN zh_TW"
AM_GNU_GETTEXT([external])

AC_CONFIG_FILES([Makefile
                 po/Makefile.in
                 src/Makefile])
AC_OUTPUT
