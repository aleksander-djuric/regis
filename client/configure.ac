AC_PREREQ([2.69])
AC_INIT([regcli], [1.0])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([regcli.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([crypto], [EVP_EncryptInit])
AC_CHECK_LIB([ssl], [SSL_library_init])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h netdb.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostbyname gethostname memmove memset select socket strdup strerror strncasecmp strstr])

CFLAGS="$CFLAGS -Wall -O2"
AC_CONFIG_FILES([Makefile
                 tools/Makefile])
AC_OUTPUT
