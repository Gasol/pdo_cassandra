
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

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>
#include <iostream>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace org::apache::cassandra;
using namespace std;

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
		H->client = &client;
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
	//dbh->methods = &sqlite_methods;

	return ret;
}
/* }}} */

pdo_driver_t pdo_cassandra_driver = {
	PDO_DRIVER_HEADER(cassandra),
	pdo_cassandra_handle_factory
};
