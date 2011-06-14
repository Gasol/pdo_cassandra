--TEST--
Cassandra
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'cassandra_pdo_test.inc');
CassandraPDOTest::skipIfNotLoaded();
?>
--FILE--
<?php
echo "pdo_cassandra extension is available";
--EXPECT--
pdo_cassandra extension is available
