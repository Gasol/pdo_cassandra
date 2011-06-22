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

$db->exec("CREATE KEYSPACE $keyspace WITH strategy_class = 'org.apache.cassandra.locator.SimpleStrategy' AND strategy_options:replication_factor = 1");
$db->exec("USE $keyspace");
$db->exec("CREATE COLUMNFAMILY $columnFamily (KEY text PRIMARY KEY, col1 text, col2 int)");
$db->exec("INSERT INTO $columnFamily (KEY, col1, col2) VALUES (2, 'text2', 2)");
$stmt = $db->prepare("INSERT INTO $columnFamily (KEY, col1, col2) VALUES (?, ?, ?)");
$stmt->bindValue(1, 1, PDO::PARAM_STR);
$stmt->bindValue(2, 'text1');
//$stmt->bindValue(3, 2, PDO::PARAM_INT);
$stmt->bindValue(3, 1);
$stmt->execute();
$stmt = $db->query("SELECT * FROM $columnFamily");
do {
    $data = $stmt->fetchAll();
    print_r($data);
    //var_dump($data);
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
