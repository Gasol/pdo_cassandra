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
echo $db->exec("CREATE COLUMNFAMILY $columnFamily (KEY text PRIMARY KEY, col1 text)") . PHP_EOL;
echo $db->exec("INSERT INTO $columnFamily (KEY, col1) VALUES ('a', 'text1')") . PHP_EOL;
echo $db->exec("INSERT INTO $columnFamily (KEY, col1, col2) VALUES ('b', 'text1', 'text2')") . PHP_EOL;
$stmt = $db->query("SELECT * FROM $columnFamily");
do {
    $data = $stmt->fetchAll();
    var_dump($data);
} while ($stmt->nextRowset());
var_dump($stmt->fetchAll());
$stmt = $db->query("SELECT * FROM $columnFamily WHERE KEY = 'a'");
var_dump($stmt->fetchAll());
$stmt = $db->query("SELECT * FROM $columnFamily WHERE KEY = 'b'");
var_dump($stmt->fetchAll());
echo $db->exec("DROP KEYSPACE $keyspace") . PHP_EOL;

?>
--EXPECT--
0
0
0
0
0
array(1) {
  [0]=>
  array(4) {
    ["KEY"]=>
    string(1) "a"
    [0]=>
    string(1) "a"
    ["col1"]=>
    string(5) "text1"
    [1]=>
    string(5) "text1"
  }
}
array(1) {
  [0]=>
  array(6) {
    ["KEY"]=>
    string(1) "b"
    [0]=>
    string(1) "b"
    ["col1"]=>
    string(5) "text1"
    [1]=>
    string(5) "text1"
    ["col2"]=>
    string(5) "text2"
    [2]=>
    string(5) "text2"
  }
}
array(1) {
  [0]=>
  array(4) {
    ["KEY"]=>
    string(1) "a"
    [0]=>
    string(1) "a"
    ["col1"]=>
    string(5) "text1"
    [1]=>
    string(5) "text1"
  }
}
array(1) {
  [0]=>
  array(6) {
    ["KEY"]=>
    string(1) "b"
    [0]=>
    string(1) "b"
    ["col1"]=>
    string(5) "text1"
    [1]=>
    string(5) "text1"
    ["col2"]=>
    string(5) "text2"
    [2]=>
    string(5) "text2"
  }
}
0
