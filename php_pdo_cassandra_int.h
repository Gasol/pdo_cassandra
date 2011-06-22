/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Gasol Wu <gasol.wu@gmail.com>                                |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_PDO_CASSANDRA_INT_H
#define PHP_PDO_CASSANDRA_INT_H

#include "Cassandra.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <iostream>

#ifdef DEBUG_CASSANDRA
#define DEBUG_OUTPUT(s) printf("%s\n", s);
#else
#define DEBUG_OUTPUT(s)
#endif

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace org::apache::cassandra;
using namespace std;

typedef struct {
	CassandraClient client;
	boost::shared_ptr<TTransport> transport;
    string *keyspace;
    map<string, KsDef> *ks_defs;
} pdo_cassandra_db_handle;

typedef struct {
	pdo_cassandra_db_handle 	*H;
	CqlResult *result;
    string *column_family;
	int index;
	bool ignore_column_count;
} pdo_cassandra_stmt;

extern pdo_driver_t pdo_cassandra_driver;
extern struct pdo_stmt_methods cassandra_stmt_methods;

BEGIN_EXTERN_C()
char* scan_columnfamily(char *cql);
int64_t deserializeLong(char *str);
char* ltoa(long N, char *str);
END_EXTERN_C()

#endif // PHP_PDO_CASSANDRA_INT_H
