
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

static int pdo_cassandra_stmt_dtor(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
	efree(S);
	return 1;
}

static int pdo_cassandra_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
	return 1;
}

static int pdo_cassandra_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
	return 1;
}

static int pdo_cassandra_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
	return 1;
}

static int pdo_cassandra_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
	return 1;
}

static int pdo_cassandra_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC)
{
	return 1;
}

static int pdo_cassandra_stmt_col_meta(pdo_stmt_t *stmt, long colno, zval *return_value TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
	return SUCCESS;
}

static int pdo_cassandra_stmt_cursor_closer(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
	return 1;
}

struct pdo_stmt_methods cassandra_stmt_methods = {
	pdo_cassandra_stmt_dtor,
	pdo_cassandra_stmt_execute,
	pdo_cassandra_stmt_fetch,
	pdo_cassandra_stmt_describe,
	pdo_cassandra_stmt_get_col,
	pdo_cassandra_stmt_param_hook,
	NULL, /* set_attr */
	NULL, /* get_attr */
	pdo_cassandra_stmt_col_meta,
	NULL, /* next_rowset */
	pdo_cassandra_stmt_cursor_closer
};
