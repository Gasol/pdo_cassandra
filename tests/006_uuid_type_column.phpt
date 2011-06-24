--TEST--
Cassandra UUIDType(uuid) Column
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
$db->exec("CREATE COLUMNFAMILY $columnFamily (KEY text PRIMARY KEY) WITH comparator = uuid");

$columns = array();
$values = array();
for ($i = 0; $i < 20; $i++) {
    $columns[] = "'column$i'";
    $values[] = "'value$i'";
}
$db->exec("INSERT INTO $columnFamily (KEY, '3abdd164-5f35-4324-9f89-d6afa1e5fe24', '217aae67-8dbf-4a3c-b261-604f19abd996', '776272a4-9dab-432f-bc4e-12a425e5d3af') VALUES ('row_a', '', '', '')");
$stmt = $db->query("SELECT * FROM $columnFamily");
var_dump($stmt->fetchAll());
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;
$db->exec("DROP KEYSPACE $keyspace") . PHP_EOL;
?>
--EXPECT--
array(1) {
  [0]=>
  array(4) {
    ["KEY"]=>
    string(5) "row_a"
    ["217aae67-8dbf-4a3c-b261-604f19abd996"]=>
    string(0) ""
    ["3abdd164-5f35-4324-9f89-d6afa1e5fe24"]=>
    string(0) ""
    ["776272a4-9dab-432f-bc4e-12a425e5d3af"]=>
    string(0) ""
  }
}
columnCount: 4
rowCount: 1
