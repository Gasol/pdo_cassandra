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

$db = CassandraPDOTest::factory();
$keyspace = getenv('PDO_CASSANDRA_TEST_KEYSPACE');
$columnFamily = getenv('PDO_CASSANDRA_TEST_COLUMNFAMILY');
try {
    $db->exec("DROP KEYSPACE $keyspace");
} catch (PDOException $e) {
    // ignore
    // echo $e->getMessage() . PHP_EOL;
}

echo $db->exec("CREATE KEYSPACE $keyspace WITH strategy_class = 'org.apache.cassandra.locator.SimpleStrategy' AND strategy_options:replication_factor = 1") . PHP_EOL;
echo $db->exec("USE $keyspace") . PHP_EOL;
echo $db->exec("CREATE COLUMNFAMILY $columnFamily (KEY text PRIMARY KEY, col text)") . PHP_EOL;
echo $db->exec("INSERT INTO $columnFamily (KEY, col) VALUES ('testkey', 'testtext')") . PHP_EOL;
$stmt = $db->query("SELECT KEY, col FROM $columnFamily");
var_dump($stmt->fetchAll());
$stmt = $db->query("SELECT KEY, col FROM $columnFamily WHERE KEY = 'testkey'");
var_dump($stmt->fetchAll());
echo $db->exec("DROP KEYSPACE $keyspace") . PHP_EOL;

?>
--EXPECT--
0
0
0
0
array(1) {
  [0]=>
  array(4) {
    ["KEY"]=>
    string(7) "testkey"
    [0]=>
    string(7) "testkey"
    ["col"]=>
    string(8) "testtext"
    [1]=>
    string(8) "testtext"
  }
}
array(1) {
  [0]=>
  array(4) {
    ["KEY"]=>
    string(7) "testkey"
    [0]=>
    string(7) "testkey"
    ["col"]=>
    string(8) "testtext"
    [1]=>
    string(8) "testtext"
  }
}
0
