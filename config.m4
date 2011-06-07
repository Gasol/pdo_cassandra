dnl $Id$
dnl config.m4 for extension pdo_cassandra
dnl vim: set sw=2 ts=2 et:

PHP_ARG_WITH(pdo-cassandra, for pdo_cassandra support,
[  --with-pdo-cassandra             Include pdo_cassandra support])

if test "$PHP_PDO_CASSANDRA" != "no"; then
  PHP_REQUIRE_CXX()

  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

	AC_CACHE_CHECK([for PDO Includes], pdo_cv_inc_path, [
		if test -f $phpincludedir/ext/pdo/php_pdo_driver.h; then
			pdo_cv_inc_path=$phpincludedir/ext
		else
			AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
		fi
	])

  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="Cassandra.h"  # you most likely want to change this
  if test -r $PHP_PDO_CASSANDRA/$SEARCH_FOR; then # path given as parameter
    PDO_CASSANDRA_DIR=$PHP_PDO_CASSANDRA
  else # search default path list
    AC_MSG_CHECKING([for pdo_cassandra files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        PDO_CASSANDRA_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$PDO_CASSANDRA_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please generate the cassandra library from thrift IDL])
  fi

  PHP_ADD_INCLUDE($PDO_CASSANDRA_DIR)

  dnl # --with-pdo_cassandra -> check for lib and symbol presence
  LIBNAME=cassandra
  dnl LIBSYMBOL=pdo_cassandra # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PDO_CASSANDRA_DIR/lib, PDO_CASSANDRA_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PDO_CASSANDRALIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong pdo_cassandra lib version or lib not found])
  dnl ],[
  dnl   -L$PDO_CASSANDRA_DIR/lib -lm
  dnl ])
  dnl
  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PDO_CASSANDRA_DIR, PDO_CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, , PDO_CASSANDRA_SHARED_LIBADD)
  AC_DEFINE(HAVE_PDO_CASSANDRALIB,1,[ ])
  PHP_SUBST(PDO_CASSANDRA_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pdo_cassandra, pdo_cassandra.cpp, $ext_shared,, -I$pdo_cv_inc_path)
fi
