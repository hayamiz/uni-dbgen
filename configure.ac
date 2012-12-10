AC_PREREQ(2.59)

AC_INIT(uni-dbgen, 0.0.1, haya@tkl.iis.u-tokyo.ac.jp)
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

AC_PROG_LIBTOOL

AC_CONFIG_FILES([Makefile
	src/Makefile])
dnl ******************************
dnl Check for standard headers
dnl ******************************

AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_CHECK_HEADER(inttypes.h, [], [AC_MSG_ERROR([inttypes.h required.])])
AC_CHECK_HEADER(getopt.h, [], [AC_MSG_ERROR([getopt.h required.])])

dnl ******************************
dnl Check for standard programs
dnl ******************************

AC_PROG_GREP

dnl **************************************************************
dnl Check for GLib
dnl **************************************************************

PKG_CHECK_MODULES([GLIB],
                  [glib-2.0 gthread-2.0],
                  [ac_glib_2_0_available="yes"],
                  [ac_glib_2_0_available="no"])

if test "$ac_glib_2_0_available" = "no"; then
  AC_MSG_ERROR([GLib is required.])
fi

AC_OUTPUT

echo
echo "Configure Result:"
echo
echo "== required headers and libraries =="
echo "       GLib : $ac_glib_2_0_available"
echo
