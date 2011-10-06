--TEST--
Cassandra BytesType(bytea) Column
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

function strToHex($str) {
    $hex = '';
    for ($i = 0; $i < strlen($str); $i++) {
        $hex .= dechex(ord($str[$i]));
    }
    return $hex;
}

$db->exec("CREATE KEYSPACE $keyspace WITH " .
    "strategy_class = 'org.apache.cassandra.locator.SimpleStrategy' AND " .
    "strategy_options:replication_factor = 1");
$db->exec("USE $keyspace");
$db->exec("CREATE COLUMNFAMILY $columnFamily (KEY text PRIMARY KEY) " .
    "WITH comparator = bytea");

$chinese = strToHex('中文測試');
$db->exec("INSERT INTO $columnFamily (KEY, '30', '61', '41', '$chinese') " .
    "VALUES ('row_a', '', '', '', '')");
$stmt = $db->query("SELECT * FROM $columnFamily");
var_dump($stmt->fetchAll());
echo 'columnCount: ' . $stmt->columnCount() . PHP_EOL;
echo 'rowCount: ' . $stmt->rowCount() . PHP_EOL;
$db->exec("DROP KEYSPACE $keyspace") . PHP_EOL;
?>
--EXPECT--
array(1) {
  [0]=>
  array(5) {
    ["KEY"]=>
    string(5) "row_a"
    [0]=>
    string(0) ""
    ["A"]=>
    string(0) ""
    ["a"]=>
    string(0) ""
    ["中文測試"]=>
    string(0) ""
  }
}
columnCount: 5
rowCount: 1
