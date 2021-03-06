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

/* If you declare any globals in php_pdo_cassandra.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(pdo_cassandra)
*/

/* True global resources - no need for thread safety here */
static int le_pdo_cassandra;

/* {{{ pdo_cassandra_functions[]
 *
 * Every user visible function must have an entry in pdo_cassandra_functions[].
 */
const zend_function_entry pdo_cassandra_functions[] = {
	{NULL, NULL, NULL}	/* Must be the last line in pdo_cassandra_functions[] */
};
/* }}} */

/* {{{ pdo_cassandra_module_entry
 */
zend_module_entry pdo_cassandra_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"pdo_cassandra",
	pdo_cassandra_functions,
	PHP_MINIT(pdo_cassandra),
	PHP_MSHUTDOWN(pdo_cassandra),
	NULL,		/* Replace with NULL if there's nothing to do at request start */
	NULL,		/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(pdo_cassandra),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDO_CASSANDRA
ZEND_GET_MODULE(pdo_cassandra)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("pdo_cassandra.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_pdo_cassandra_globals, pdo_cassandra_globals)
    STD_PHP_INI_ENTRY("pdo_cassandra.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_pdo_cassandra_globals, pdo_cassandra_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_pdo_cassandra_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_pdo_cassandra_init_globals(zend_pdo_cassandra_globals *pdo_cassandra_globals)
{
	pdo_cassandra_globals->global_value = 0;
	pdo_cassandra_globals->global_string = NULL;
}
*/
/* }}} */

void cassandra_log_err(const char *message)
{
	php_log_err(const_cast<char *>(message));
}

/* {{{ PHP_MINIT_FUNCTION
 */

PHP_MINIT_FUNCTION(pdo_cassandra)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	GlobalOutput.setOutputFunction(&cassandra_log_err);
	return php_pdo_register_driver(&pdo_cassandra_driver);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pdo_cassandra)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	php_pdo_unregister_driver(&pdo_cassandra_driver);
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdo_cassandra)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pdo_cassandra support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
