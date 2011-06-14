--TEST--
Cassandra PDO->__construct()
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'cassandra_pdo_test.inc');
CassandraPDOTest::skipIfNotLoaded();
?>
--FILE--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'cassandra_pdo_test.inc');
function tryandcatch($offset, $code) {
    try {
        eval($code);
        assert(sprintf("[%03d] %s\n", $offset, 'should have fails') != '');
    } catch (PDOException $e) {
        return sprintf("[%03d] %s\n", $offset, $e->getMessage());
    }
    return '';
}
echo tryandcatch(1, '$db = new PDO("CASSANDRA");');
echo tryandcatch(2, '$db = new PDO("CASSANDRA:");');
echo tryandcatch(3, '$db = new PDO("cassandra");');
echo tryandcatch(4, '$db = new PDO("cassandra ");');
echo tryandcatch(5, '$db = new PDO("cassandra:");');
echo tryandcatch(6, '$db = new PDO("cassandra:host=example");');

$db = CassandraPDOTest::factory();

?>
--EXPECT--
[001] invalid data source name
[002] could not find driver
[003] invalid data source name
[004] invalid data source name
[005] invalid data source name
[006] Could not resolve host for client socket.
