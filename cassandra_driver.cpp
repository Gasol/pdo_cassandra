
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

static int cassandra_handle_closer(pdo_dbh_t *dbh TSRMLS_DC) /* {{{ */
{
	pdo_cassandra_db_handle *H = (pdo_cassandra_db_handle *)dbh->driver_data;

	if (H) {
		if (H->transport->isOpen()) {
			H->transport->close();
		}
		pefree(H, dbh->is_persistent);
		dbh->driver_data = NULL;
	}
	return 0;
}
/* }}} */

static int cassandra_handle_preparer(pdo_dbh_t *dbh, const char *sql, long sql_len, pdo_stmt_t *stmt, zval *driver_options TSRMLS_DC)
{
	return 0;
}

static long cassandra_handle_doer(pdo_dbh_t *dbh, const char *sql, long sql_len TSRMLS_DC)
{
	pdo_cassandra_db_handle *H = (pdo_cassandra_db_handle *)dbh->driver_data;

	CqlResult result;
	try {
		H->client.execute_cql_query(result, sql, Compression::NONE);
	} catch(InvalidRequestException &e) {
		char *message = new char[e.why.size()+1];
		strcpy(message, e.why.c_str());
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
	return 1;
}

static int cassandra_handle_begin(pdo_dbh_t *dbh TSRMLS_DC)
{
	char message[] = "unsupported function";
	zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, message);
	return 0;
}

static int cassandra_handle_commit(pdo_dbh_t *dbh TSRMLS_DC)
{
	char message[] = "unsupported function";
	zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, message);
	return 0;
}

static int cassandra_handle_rollback(pdo_dbh_t *dbh TSRMLS_DC)
{
	char message[] = "unsupported function";
	zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, message);
	return 0;
}

static int pdo_cassandra_set_attr(pdo_dbh_t *dbh, long attr, zval *val TSRMLS_DC)
{
	return 0;
}

static char* pdo_cassandra_last_insert_id(pdo_dbh_t *dbh, const char *name, unsigned int *len TSRMLS_DC)
{
	char message[] = "unsupported function";
	zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, message);
	return "-1";
}

static int pdo_cassandra_fetch_error_func(pdo_dbh_t *dbh, pdo_stmt_t *stmt, zval *info TSRMLS_DC)
{
	return 1;
}

static int pdo_cassandra_get_attribute(pdo_dbh_t *dbh, long attr, zval *return_value TSRMLS_DC)
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
	cassandra_handle_begin,
	cassandra_handle_commit,
	cassandra_handle_rollback,
	pdo_cassandra_set_attr,
	pdo_cassandra_last_insert_id,
	pdo_cassandra_fetch_error_func,
	pdo_cassandra_get_attribute,
	NULL,	/* check_liveness: not needed */
	get_driver_methods,
	pdo_cassandra_request_shutdown
};

static int pdo_cassandra_handle_factory(pdo_dbh_t *dbh, zval *driver_options TSRMLS_DC) /* {{{ */
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

	boost::shared_ptr<TSocket> socket(new TSocket(host, port));
	boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	CassandraClient client(protocol);

	try {
		transport->open();
		H->client = client;
		H->transport = transport;
		H->socket = socket;
		if (strlen(dbh->username) && strlen(dbh->password)) {
			AuthenticationRequest auth_request;
			auth_request.credentials.insert(pair<string, string>("username", dbh->username));
			auth_request.credentials.insert(pair<string, string>("password", dbh->password));
			client.login(auth_request);
		}
		//transport->close();
	} catch (InvalidRequestException &e) {
		cout << "Error: %s\n" << e.why.c_str();
	}


	dbh->alloc_own_columns = 1;
	dbh->max_escaped_char_length = 2;

	ret = 1;

cleanup:
	dbh->methods = &cassandra_methods;

	return ret;
}
/* }}} */

pdo_driver_t pdo_cassandra_driver = {
	PDO_DRIVER_HEADER(cassandra),
	pdo_cassandra_handle_factory
};
