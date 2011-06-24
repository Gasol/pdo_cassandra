--TEST--
Cassandra LongType(bigint) Column
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
}

$db->exec("CREATE KEYSPACE $keyspace WITH strategy_class = 'org.apache.cassandra.locator.SimpleStrategy' AND strategy_options:replication_factor = 1") . PHP_EOL;
$db->exec("USE $keyspace") . PHP_EOL;
$db->exec("CREATE COLUMNFAMILY $columnFamily (KEY text PRIMARY KEY) WITH comparator = bigint");

$columns = array();
$values = array();
for ($i = 32767; $i < 32800; $i+=3) {
    $columns[] = $i;
    $values[] = "'value$i'";
}
$db->exec("INSERT INTO $columnFamily (KEY, " . implode(', ', $columns) . ") VALUES ('row_a', " . implode(', ', $values) . ")");
$columns = array();
$values = array();
for ($i = -10; $i < 10; $i++) {
    $columns[] = $i;
    $values[] = "'value$i'";
}
$db->exec("INSERT INTO $columnFamily (KEY, " . implode(', ', $columns) . ") VALUES ('row_b', " . implode(', ', $values) . ")");
$stmt = $db->query("SELECT 8..32770 FROM $columnFamily");
var_dump($stmt->fetchAll());
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;

$stmt = $db->query("SELECT 32770..32780 FROM $columnFamily WHERE KEY = row_a");
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;
var_dump($stmt->fetch());
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
$stmt = $db->query("SELECT FIRST 5 32770..32780 FROM $columnFamily WHERE KEY = row_a");
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;
var_dump($stmt->fetch());
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
$stmt = $db->query("SELECT FIRST 5 REVERSED 5..-5 FROM $columnFamily WHERE KEY = 'row_b'");
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;
var_dump($stmt->fetch());
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;

$db->exec("DROP KEYSPACE $keyspace") . PHP_EOL;
?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [8]=>
    string(6) "value8"
    [9]=>
    string(6) "value9"
  }
  [1]=>
  array(2) {
    [32767]=>
    string(10) "value32767"
    [32770]=>
    string(10) "value32770"
  }
}
columnCount: 2
rowCount: 2
columnCount: 0
rowCount: 1
array(4) {
  [32770]=>
  string(10) "value32770"
  [32773]=>
  string(10) "value32773"
  [32776]=>
  string(10) "value32776"
  [32779]=>
  string(10) "value32779"
}
columnCount: 4
columnCount: 0
rowCount: 1
array(4) {
  [32770]=>
  string(10) "value32770"
  [32773]=>
  string(10) "value32773"
  [32776]=>
  string(10) "value32776"
  [32779]=>
  string(10) "value32779"
}
columnCount: 4
columnCount: 0
rowCount: 1
array(5) {
  [5]=>
  string(6) "value5"
  [4]=>
  string(6) "value4"
  [3]=>
  string(6) "value3"
  [2]=>
  string(6) "value2"
  [1]=>
  string(6) "value1"
}
columnCount: 5
