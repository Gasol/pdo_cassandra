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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
BEGIN_EXTERN_C()
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
END_EXTERN_C()
#include "php_pdo_cassandra.h"
#include "php_pdo_cassandra_int.h"
#include "zend_exceptions.h"

static int cassandra_handle_closer(pdo_dbh_t *dbh TSRMLS_DC)
{
	pdo_cassandra_db_handle *H = (pdo_cassandra_db_handle *)dbh->driver_data;

	if (H) {
		if (H->transport != NULL && H->transport->isOpen()) {
			H->transport->close();
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
	return 0;
}

static int cassandra_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	pdo_cassandra_db_handle *H = (pdo_cassandra_db_handle *)dbh->driver_data;
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt *) ecalloc(1, sizeof(pdo_cassandra_stmt));

	S->H = H;
	stmt->driver_data = S;
	stmt->methods = &cassandra_stmt_methods;
	stmt->supports_placeholders = PDO_PLACEHOLDER_NONE;
	stmt->column_count = 0;

	return 1;
}

static long cassandra_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_cassandra_db_handle *H = (pdo_cassandra_db_handle *)dbh->driver_data;

	CqlResult result;
	try {
		H->client.execute_cql_query(result, sql, Compression::NONE);
	} catch(InvalidRequestException &e) {
		char *message = estrdup(e.why.c_str());
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, message);
		return -1;
	}

	switch (result.type) {
		case CqlResultType::ROWS:
			return result.rows.size();
		case CqlResultType::INT:
			return result.num;
		case CqlResultType::VOID:
		default:
			return 0;
	}
}

static int cassandra_handle_quoter(pdo_dbh_t *dbh, const char *unquoted, int unquotedlen, char **quoted, int *quotedlen, enum pdo_param_type paramtype  TSRMLS_DC)
{
    *quoted = (char *) safe_emalloc(2, unquotedlen, 3);
    snprintf(*quoted, 2 * unquotedlen + 3, "'%s'", unquoted);
    *quotedlen = strlen(*quoted);
	return 1;
}

static int pdo_cassandra_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	return 1;
}

static int pdo_cassandra_get_attribute(pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC)
{
	return 1;
}

static int pdo_cassandra_set_attribute(pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC)
{
	return 1;
}

static const zend_function_entry *get_driver_methods(pdo_dbh_t *dbh, int kind TSRMLS_DC)
{
	switch (kind) {
		default:
			return NULL;
	}
}

static void pdo_cassandra_request_shutdown(pdo_dbh_t *dbh TSRMLS_DC)
{
}

static struct pdo_dbh_methods cassandra_methods = {
	cassandra_handle_closer,
	cassandra_handle_preparer,
	cassandra_handle_doer,
	cassandra_handle_quoter,
	NULL, /* begin */
	NULL, /* commit */
	NULL, /* rollback */
	NULL, /* pdo_cassandra_set_attribute, */
	NULL, /* last_insert_id */
	NULL, /* pdo_cassandra_fetch_error_func, */
	NULL, /* pdo_cassandra_get_attribute, */
	NULL,	/* check_liveness */
	get_driver_methods,
	NULL /* pdo_cassandra_request_shutdown */
};

static int pdo_cassandra_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC)
{
	pdo_cassandra_db_handle *H;
	int i, ret = 0;
	char *host = NULL;
	char *keyspace = NULL;
	unsigned int port = 9160;
	long timeout = 60;

	struct pdo_data_src_parser vars[] = {
		{"host", NULL, 0},
		{"keyspace", NULL, 0},
		{"port", "9160", 0},
	};

	php_pdo_parse_data_source(dbh->data_source, dbh->data_source_len, vars, 3);

	H = (pdo_cassandra_db_handle*) pecalloc(1, sizeof(pdo_cassandra_db_handle), dbh->is_persistent);

	dbh->driver_data = H;

	host = vars[0].optval;
	keyspace = vars[1].optval;
	if (vars[2].optval) {
		port = atoi(vars[2].optval);
	}

    if (host == NULL) {
        zend_throw_exception(php_pdo_get_exception(), "invalid data source name", 0 TSRMLS_CC);
        goto cleanup;
    }

	try {
        boost::shared_ptr<TSocket> socket(new TSocket(host, port));
        boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        CassandraClient client(protocol);

		H->client = client;
		H->transport = transport;
		transport->open();

		if (dbh->username && dbh->password) {
			AuthenticationRequest auth_request;
			auth_request.credentials.insert(pair<string, string>("username", dbh->username));
			auth_request.credentials.insert(pair<string, string>("password", dbh->password));
			client.login(auth_request);
		}

		if (keyspace) {
			CqlResult result;
			char *cql = new char[strlen(keyspace)+5];
			sprintf(cql, "USE %s\0", keyspace);
			H->client.execute_cql_query(result, cql, Compression::NONE);
			delete [] cql;
		}
	} catch (InvalidRequestException &e) {
		char *message = estrdup(e.why.c_str());
		zend_throw_exception(php_pdo_get_exception(), message, 0 TSRMLS_CC);
		goto cleanup;
	} catch (TTransportException &e) {
		char *message = estrdup(e.what());
		zend_throw_exception(php_pdo_get_exception(), message, 0 TSRMLS_CC);
		goto cleanup;
	} catch (TApplicationException &e) {
		char *message = estrdup(e.what());
		zend_throw_exception(php_pdo_get_exception(), message, 0 TSRMLS_CC);
		goto cleanup;
    }

	dbh->alloc_own_columns = 1;
	dbh->max_escaped_char_length = 2;
    dbh->default_fetch_type = PDO_FETCH_ASSOC;

	ret = 1;

cleanup:
	for (i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
		if (vars[i].freeme) {
			efree(vars[i].optval);
		}
	}

	dbh->methods = &cassandra_methods;

	return ret;
}

pdo_driver_t pdo_cassandra_driver = {
	PDO_DRIVER_HEADER(cassandra),
	pdo_cassandra_handle_factory
};
