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
	if (S->result) {
		delete S->result;
	}
    if (S->column_family) {
        delete S->column_family;
    }
	efree(S);
	return 1;
}

static int pdo_cassandra_stmt_execute(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
	pdo_cassandra_db_handle *H = S->H;

	CqlResult *result = new CqlResult();
	try {
        if (H->ks_defs->find(*H->keyspace) == H->ks_defs->end()) {
            KsDef ksdef;
            H->client.describe_keyspace(ksdef, *H->keyspace);
            (*H->ks_defs)[*H->keyspace] = ksdef;
        }
        *S->column_family = scan_columnfamily(stmt->active_query_string);
		H->client.execute_cql_query(*result, stmt->active_query_string, Compression::NONE);
	} catch(InvalidRequestException &e) {
		char *message = const_cast<char *>(e.why.c_str());
		zend_throw_exception_ex(php_pdo_get_exception(), 0 TSRMLS_CC, message);
		return 0;
	}
	switch (result->type) {
		case CqlResultType::ROWS:
			S->result = result;
			S->index = -1;
			S->ignore_column_count = false;
			stmt->row_count = result->rows.size();
			if (result->rows.size() > 0) {
				stmt->column_count = result->rows[0].columns.size();
			}
			break;
		case CqlResultType::INT:
			stmt->row_count = result->num;
			break;
		case CqlResultType::VOID:
			stmt->row_count = 0;
			break;
	}
	return 1;
}

static int pdo_cassandra_stmt_param_hook(pdo_stmt_t *stmt, struct pdo_bound_param_data *param,
		enum pdo_param_event event_type TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;

    if (S && param->is_param) {
        switch (event_type) {
            case PDO_PARAM_EVT_ALLOC:
                cout << "PDO_PARAM_EVT_ALLOC" << endl;
                break;
            case PDO_PARAM_EVT_FREE:
                cout << "PDO_PARAM_EVT_FREE" << endl;
                break;
            case PDO_PARAM_EVT_EXEC_PRE:
                cout << "PDO_PARAM_EVT_EXEC_PRE" << endl;
                cout << "paramno: " << param->paramno << endl;
                cout << "paramtype: " << param->param_type << endl;
                cout << "parameter: " << param->parameter << endl;
                switch (param->param_type) {
                    case PDO_PARAM_STR:
                        convert_to_string(param->parameter);
                        break;
                }
                break;
            case PDO_PARAM_EVT_EXEC_POST:
                cout << "PDO_PARAM_EVT_EXEC_POST" << endl;
                break;
            case PDO_PARAM_EVT_FETCH_PRE:
                cout << "PDO_PARAM_EVT_FETCH_PRE" << endl;
                break;
            case PDO_PARAM_EVT_FETCH_POST:
                cout << "PDO_PARAM_EVT_FETCH_POST" << endl;
                break;
            case PDO_PARAM_EVT_NORMALIZE:
                cout << "PDO_PARAM_EVT_NORMALIZE" << endl;
                break;
            default:
                cout << "event_type: " << event_type << endl;
        }
    }
	return 1;
}

static int pdo_cassandra_stmt_fetch(pdo_stmt_t *stmt,
	enum pdo_fetch_orientation ori, long offset TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;

	if (++S->index >= S->result->rows.size()) {
		return 0;
	}

	CqlRow row = S->result->rows[S->index];
	if (S->index > 0) {
		CqlRow previous_row = S->result->rows[S->index - 1];
		if (row.columns.size() != previous_row.columns.size()) {
			if (S->ignore_column_count) {
				S->ignore_column_count = false;
			} else {
				--S->index;
				return 0;
			}
		}
	}
	return 1;
}

static int pdo_cassandra_stmt_describe(pdo_stmt_t *stmt, int colno TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;

	CqlRow row = S->result->rows[S->index + 1];
	if (colno >= row.columns.size()) {
		return 0;
	}

    pdo_cassandra_db_handle *H = S->H;
	Column col = row.columns[colno];
    char *name = NULL;
    pdo_param_type param_type;

    map<string, KsDef>::iterator ks_it = H->ks_defs->find(*H->keyspace);
    if (ks_it != H->ks_defs->end()) {
        KsDef ksdef = ks_it->second;
        for (vector<CfDef>::iterator cf_it = ksdef.cf_defs.begin(); cf_it != ksdef.cf_defs.end(); cf_it++) {
            CfDef cfdef = *cf_it;
            if (cfdef.name == *S->column_family) {
                if (cf_it->comparator_type == "org.apache.cassandra.db.marshal.UTF8Type" ||
                        cf_it->comparator_type == "org.apache.cassandra.db.marshal.AsciiType") {
                    name = estrdup(col.name.c_str());
                    param_type = PDO_PARAM_STR;
                }
                /*
                for (vector<ColumnDef>::iterator column_it = cf_it->column_metadata.begin(); column_it != cf_it->column_metadata.end(); column_it++) {
                    ColumnDef column_def = *column_it;
                    cout << "column name: " << column_def.name << endl;
                    cout << "column validation_class: " << column_def.validation_class << endl;
                }
                */
            }
        }
    }

	pdo_column_data *col_data = &stmt->columns[colno];

	col_data->name = name;
	col_data->namelen = strlen(name);
	col_data->maxlen = 0xffffffff;
	col_data->precision = 0;
	col_data->param_type = param_type;

	return 1;
}

static int pdo_cassandra_stmt_get_col(pdo_stmt_t *stmt, int colno, char **ptr, unsigned long *len, int *caller_frees TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;
    pdo_cassandra_db_handle *H = S->H;

	CqlRow row = S->result->rows[S->index];
	if (colno >= row.columns.size()) {
		return 0;
	}

	Column col = row.columns[colno];

    char *a = "test";
    map<string, KsDef>::iterator ks_it = H->ks_defs->find(*H->keyspace);
    if (ks_it != H->ks_defs->end()) {
        KsDef ksdef = ks_it->second;
        for (vector<CfDef>::iterator cf_it = ksdef.cf_defs.begin(); cf_it != ksdef.cf_defs.end(); cf_it++) {
            CfDef cfdef = *cf_it;
            if (cfdef.name == *S->column_family) {
                for (vector<ColumnDef>::iterator column_it = cf_it->column_metadata.begin(); column_it != cf_it->column_metadata.end(); column_it++) {
                    ColumnDef column_def = *column_it;
                    if (col.name == column_def.name) {
                        if (column_def.validation_class == "org.apache.cassandra.db.marshal.UTF8Type" ||
                                column_def.validation_class == "org.apache.cassandra.db.marshal.AsciiType") {
                            *ptr = const_cast<char *>(col.value.c_str());
                        } else {
                            *ptr = a;
                        }
                    } else if (col.name == "KEY") {
                        if (column_def.validation_class == "org.apache.cassandra.db.marshal.UTF8Type" ||
                                column_def.validation_class == "org.apache.cassandra.db.marshal.AsciiType") {
                            *ptr = const_cast<char *>(col.value.c_str());
                        } else {
                            *ptr = a;
                        }
                    }
                }
            }
        }
    }

	*len = strlen(*ptr);

	return 1;
}

static int pdo_cassandra_stmt_col_meta(pdo_stmt_t *stmt, long colno, zval *return_value TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;

	if (!stmt->executed || !S->result) {
		return FAILURE;
	}
	if (colno >= S->result->rows.size()) {
		return FAILURE;
	}
	array_init(return_value);
    /*
    zval *flags;
	pdo_cassandra_db_handle *H = S->H;

    map<string, KsDef>::iterator it = H->ks_defs->find(*H->keyspace);
    if (it == H->ks_defs->end()) {
        return FAILURE;
    }

    array_init(return_value);
    MAKE_STD_ZVAL(flags);
    array_init(flags);

    KsDef ksdef = it->second;
    for (vector<CfDef>::iterator it = ksdef.cf_defs.begin(); it != ksdef.cf_defs.end(); it++) {
        CfDef cfdef = *it;
        cout << "name: " << cfdef.name << endl;
        cout << "column_type: " << cfdef.column_type << endl;
        cout << "comparator_type: " << cfdef.comparator_type << endl;
        cout << "sub_comparator_type: " << cfdef.subcomparator_type << endl;
        cout << "column_metadata.size(): " << cfdef.column_metadata.size() << endl;
        for (vector<ColumnDef>::iterator it = cfdef.column_metadata.begin(); it != cfdef.column_metadata.end(); it++) {
            ColumnDef column_def = *it;
            cout << "name: " << column_def.name << endl;
            cout << "validation_class: " << column_def.validation_class << endl;
            cout << "index_type: " << column_def.index_type << endl;
            cout << "index_name: " << column_def.index_name << endl;
        }
    }
    */

	return SUCCESS;
}

static int pdo_cassandra_stmt_next_rowset(pdo_stmt_t *stmt TSRMLS_DC)
{
	pdo_cassandra_stmt *S = (pdo_cassandra_stmt*)stmt->driver_data;

	if (S->index < S->result->rows.size()) {
		stmt->column_count = S->result->rows[S->index + 1].columns.size();
		S->ignore_column_count = true;
		return 1;
	} else {
		return 0;
	}
}

static int pdo_cassandra_stmt_cursor_closer(pdo_stmt_t *stmt TSRMLS_DC)
{
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
	pdo_cassandra_stmt_next_rowset,
	NULL /* pdo_cassandra_stmt_cursor_closer */
};
