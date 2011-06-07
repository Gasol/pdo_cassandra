dnl $Id$
dnl config.m4 for extension pdo_cassandra

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(pdo_cassandra, for pdo_cassandra support,
dnl Make sure that the comment is aligned:
dnl [  --with-pdo_cassandra             Include pdo_cassandra support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(pdo_cassandra, whether to enable pdo_cassandra support,
dnl Make sure that the comment is aligned:
dnl [  --enable-pdo_cassandra           Enable pdo_cassandra support])

if test "$PHP_PDO_CASSANDRA" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-pdo_cassandra -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/pdo_cassandra.h"  # you most likely want to change this
  dnl if test -r $PHP_PDO_CASSANDRA/$SEARCH_FOR; then # path given as parameter
  dnl   PDO_CASSANDRA_DIR=$PHP_PDO_CASSANDRA
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for pdo_cassandra files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PDO_CASSANDRA_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PDO_CASSANDRA_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the pdo_cassandra distribution])
  dnl fi

  dnl # --with-pdo_cassandra -> add include path
  dnl PHP_ADD_INCLUDE($PDO_CASSANDRA_DIR/include)

  dnl # --with-pdo_cassandra -> check for lib and symbol presence
  dnl LIBNAME=pdo_cassandra # you may want to change this
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
  dnl PHP_SUBST(PDO_CASSANDRA_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pdo_cassandra, pdo_cassandra.c, $ext_shared)
fi
