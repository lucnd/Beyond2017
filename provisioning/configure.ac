#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(provisioning, 0.1, wonsik93.park@lgepartner.com)
AC_CONFIG_SRCDIR([ProvisioningApp.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall gnu foreign])

LT_INIT([dlopen])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_LIBTOOL_DLOPEN

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
# 20170202. add the option "--with-security", _ENABLE_SECURITY and ENABLE_SECURITY defined by value of "enable_security"
AC_ARG_WITH(security,
                AC_HELP_STRING([--with-security],[select _ENABLE_SECURITY or not]),
                [],
                enable_security=yes) /* must be setted 'yes' for appling gcc security options.*/
if test "x$enable_security" == "xyes"; then
   AM_CPPFLAGS="$AM_CPPFLAGS -D_ENABLE_SECURITY=1"
else
   AM_CPPFLAGS="$AM_CPPFLAGS -D_ENABLE_SECURITY=0"
fi
AM_CONDITIONAL(ENABLE_SECURITY, test "x${enable_security}" = "xyes")

# Checks for library functions.

AC_CONFIG_FILES([ Makefile ])

AC_OUTPUT
