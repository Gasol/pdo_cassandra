dnl $Id$
dnl config.m4 for extension pdo_cassandra
dnl vim: set sw=2 ts=2 et:

PHP_ARG_WITH(pdo-cassandra, for pdo_cassandra support,
[  --with-pdo-cassandra    Include pdo_cassandra support])

PHP_ARG_WITH(thrift, for thrift support,
[  --with-thrift           Include thrift support], yes, no)

if test "$PHP_PDO_CASSANDRA" != "no"; then
  PHP_REQUIRE_CXX()

  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test "$PHP_THRIFT" == "no"; then
    AC_MSG_ERROR([thrift enable needed!])
  else
    found_thrift=no
    if test "$PHP_THRIFT" = "yes" && test -x "$PKG_CONFIG" && $PKG_CONFIG --exists thrift; then
      if $PKG_CONFIG --atleast-version 0.4 thrift; then
        found_thrift=yes
        THRIFT_LIBS=`$PKG_CONFIG --libs thrift`
        THRIFT_INCS=`$PKG_CONFIG --cflags-only-I thrift`
      else
        AC_MSG_ERROR([thrift version 0.4.0 or greater required.])
      fi
    fi

    if test "$found_thrift" = "no"; then
      SEARCH_PATH="/usr/local/include/thrift /usr/include/thrift"
      SEARCH_FOR="Thrift.h"
      if test -r $PHP_THRIFT/$SEARCH_FOR; then # path given as parameter
        THRIFT_INCS="-I$PHP_THRIFT"
      else
        AC_MSG_CHECKING([for Thrift files in default path])
        for i in $SEARCH_PATH ; do
          if test -r $i/$SEARCH_FOR; then
            THRIFT_INCS="-I$i"
            AC_MSG_RESULT(found in $i)
          fi
        done
      fi

      AC_MSG_CHECKING([for Thrift library location])
      for i in $PHP_THRIFT $PHP_THRIFT/$PHP_LIBDIR /usr/local/$PHP_LIBDIR /usr/$PHP_LIBDIR; do
        if test -f $i/libthrift.a || test -f $i/libthrift.$SHLIB_SUFFIX_NAME; then
          AC_MSG_RESULT(found in $i)
          THRIFT_LIBS="-lthrift"
        fi
      done
    fi

    AC_LANG(C++)
    SAVED_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $THRIFT_INCS $THRIFT_LIBS"
    AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include<Thrift.h>
      using namespace apache::thrift;],
      [GlobalOutput.printf("hello")])],
    [PHP_EVAL_LIBLINE($THRIFT_LIBS, PDO_CASSANDRA_SHARED_LIBADD)
     PHP_EVAL_INCLINE($THRIFT_INCS)],
    [AC_MSG_ERROR([wrong thrift lib version or lib not found])])
    CPPFLAGS=$SAVED_CPPFLAGS
  fi

	AC_CACHE_CHECK([for PDO Includes], pdo_cv_inc_path, [
		if test -f $phpincludedir/ext/pdo/php_pdo_driver.h; then
			pdo_cv_inc_path=$phpincludedir/ext
		else
			AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
		fi
	])

  SEARCH_PATH="/usr/local/include /usr/include"
  SEARCH_FOR="Cassandra.h"
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

  AC_LANG(C++)
  SAVED_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS -I$PDO_CASSANDRA_DIR -L$PDO_CASSANDRA_DIR -I$THRIFT_DIR -lthrift -lcassandra -lstdc++"
  AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include<Thrift.h>
using namespace apache::thrift;],
[GlobalOutput.printf("hello")])],
  [PHP_ADD_LIBRARY(thrift, , PDO_CASSANDRA_SHARED_LIBADD)],
  [AC_MSG_ERROR([wrong thrift lib version or lib not found])])

  AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include "cassandra_constants.h"
#include <iostream>
using namespace std;
using namespace org::apache::cassandra;],
[[cassandraConstants constant;]
[cout << constant.VERSION << endl]])],
  [PHP_ADD_LIBRARY(cassandra, $PDO_CASSANDRA_DIR, PDO_CASSANDRA_SHARED_LIBADD)
  PHP_ADD_LIBPATH($PDO_CASSANDRA_DIR)],
  [AC_MSG_ERROR([wrong cassandra lib version or lib not found])])
  CPPFLAGS=$SAVED_CPPFLAGS

  PHP_ADD_LIBRARY(uuid, , PDO_CASSANDRA_SHARED_LIBADD)
  PHP_CHECK_LIBRARY(uuid, uuid_unparse,[
      PHP_ADD_LIBRARY(uuid, , PDO_CASSANDRA_SHARED_LIBADD)
  ],[
    AC_MSG_ERROR([wrong uuid lib version or lib not found])
  ])
  dnl PHP_ADD_LIBRARY(stdc++, , PDO_CASSANDRA_SHARED_LIBADD)
  AC_DEFINE(HAVE_PDO_CASSANDRALIB,1,[ ])
  PHP_SUBST(PDO_CASSANDRA_SHARED_LIBADD)

  pdo_cassandra_sources="pdo_cassandra.cpp \
                         cassandra_cql_parser.c \
                         cassandra_util.cpp \
                         cassandra_driver.cpp \
                         cassandra_statement.cpp \
                         ltoa.c"
  PHP_NEW_EXTENSION(pdo_cassandra, $pdo_cassandra_sources, $ext_shared,, -I$pdo_cv_inc_path)
  PHP_ADD_EXTENSION_DEP(pdo_cassandra, pdo)
fi
