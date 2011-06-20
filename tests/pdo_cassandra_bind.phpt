--TEST--
Cassandra PDO->bindColumn() and PDO->bindParam
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

$db->exec("CREATE KEYSPACE $keyspace WITH strategy_class = 'org.apache.cassandra.locator.SimpleStrategy' AND strategy_options:replication_factor = 1") . PHP_EOL;
$db->exec("USE $keyspace") . PHP_EOL;
$db->exec("CREATE COLUMNFAMILY $columnFamily (KEY text PRIMARY KEY, col1 text)") . PHP_EOL;
$stmt = $db->prepare("INSERT INTO $columnFamily (KEY, col1) VALUES (?, ?)");
$stmt->bindValue(1, 1, PDO::PARAM_STR);
$stmt->bindValue(2, 'text1', PDO::PARAM_STR);
$stmt->execute();
$stmt = $db->query("SELECT * FROM $columnFamily");
do {
    $data = $stmt->fetchAll();
    var_dump($data);
} while ($stmt->nextRowset());
var_dump($stmt->fetchAll());
$db->exec("DROP KEYSPACE $keyspace") . PHP_EOL;

?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["KEY"]=>
    string(1) "1"
    ["col1"]=>
    string(5) "text1"
  }
}
array(0) {
}
