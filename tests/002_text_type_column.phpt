--TEST--
Cassandra UTF8Type(text) Column
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
$db->exec("CREATE COLUMNFAMILY $columnFamily (KEY text PRIMARY KEY) WITH comparator = text");

$columns = array();
$values = array();
for ($i = 0; $i < 20; $i++) {
    $columns[] = "'column$i'";
    $values[] = "'value$i'";
}
$db->exec("INSERT INTO $columnFamily (KEY, " . implode(', ', $columns) . ") VALUES ('row_a', " . implode(', ', $values) . ")");
$columns = array();
$values = array();
for ($i = 30; $i < 40; $i++) {
    $columns[] = "'column$i'";
    $values[] = "'value$i'";
}
$db->exec("INSERT INTO $columnFamily (KEY, " . implode(', ', $columns) . ") VALUES ('row_b', " . implode(', ', $values) . ")");
$stmt = $db->query("SELECT * FROM $columnFamily");
var_dump($stmt->fetchAll());
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;

$stmt = $db->query("SELECT column10..column15 FROM $columnFamily WHERE KEY = row_a");
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;
var_dump($stmt->fetch());

$stmt = $db->query("SELECT FIRST 2 column10..column15 FROM $columnFamily WHERE KEY = row_a");
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;
var_dump($stmt->fetch());

$stmt = $db->query("SELECT FIRST 5 REVERSED column50..column30 FROM $columnFamily WHERE KEY = 'row_b'");
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;
var_dump($stmt->fetch());

$db->exec("DROP KEYSPACE $keyspace") . PHP_EOL;
?>
--EXPECT--
array(2) {
  [0]=>
  array(11) {
    ["KEY"]=>
    string(5) "row_b"
    ["column30"]=>
    string(7) "value30"
    ["column31"]=>
    string(7) "value31"
    ["column32"]=>
    string(7) "value32"
    ["column33"]=>
    string(7) "value33"
    ["column34"]=>
    string(7) "value34"
    ["column35"]=>
    string(7) "value35"
    ["column36"]=>
    string(7) "value36"
    ["column37"]=>
    string(7) "value37"
    ["column38"]=>
    string(7) "value38"
    ["column39"]=>
    string(7) "value39"
  }
  [1]=>
  array(21) {
    ["KEY"]=>
    string(5) "row_a"
    ["column0"]=>
    string(6) "value0"
    ["column1"]=>
    string(6) "value1"
    ["column10"]=>
    string(7) "value10"
    ["column11"]=>
    string(7) "value11"
    ["column12"]=>
    string(7) "value12"
    ["column13"]=>
    string(7) "value13"
    ["column14"]=>
    string(7) "value14"
    ["column15"]=>
    string(7) "value15"
    ["column16"]=>
    string(7) "value16"
    ["column17"]=>
    string(7) "value17"
    ["column18"]=>
    string(7) "value18"
    ["column19"]=>
    string(7) "value19"
    ["column2"]=>
    string(6) "value2"
    ["column3"]=>
    string(6) "value3"
    ["column4"]=>
    string(6) "value4"
    ["column5"]=>
    string(6) "value5"
    ["column6"]=>
    string(6) "value6"
    ["column7"]=>
    string(6) "value7"
    ["column8"]=>
    string(6) "value8"
    ["column9"]=>
    string(6) "value9"
  }
}
columnCount: 21
rowCount: 2
columnCount: 0
rowCount: 1
array(6) {
  ["column10"]=>
  string(7) "value10"
  ["column11"]=>
  string(7) "value11"
  ["column12"]=>
  string(7) "value12"
  ["column13"]=>
  string(7) "value13"
  ["column14"]=>
  string(7) "value14"
  ["column15"]=>
  string(7) "value15"
}
columnCount: 0
rowCount: 1
array(2) {
  ["column10"]=>
  string(7) "value10"
  ["column11"]=>
  string(7) "value11"
}
columnCount: 0
rowCount: 1
array(5) {
  ["column39"]=>
  string(7) "value39"
  ["column38"]=>
  string(7) "value38"
  ["column37"]=>
  string(7) "value37"
  ["column36"]=>
  string(7) "value36"
  ["column35"]=>
  string(7) "value35"
}
