<?php

class CassandraPDOTest
{
    public static function factory($file = NULL)
    {
        self::loadConfig($file);

        $username = getenv('PDO_CASSANDRA_TEST_USER');
        $password = getenv('PDO_CASSANDRA_TEST_PASS');
        $dsn = self::getDSN(getenv('PDO_CASSANDRA_TEST_HOST'), getenv('PDO_CASSANDRA_TEST_PORT'), getenv('PDO_CASSANDRA_TEST_KEYSPACE'));

        if (!$db = new PDO($dsn, $username, $password)) {
            die("Could not create PDO object (DSN=$dsn, user=$username\n");
        }

        return $db;
    }

    protected static function loadConfig($file = NULL)
    {
        if (getenv('PDO_CASSANDRA_CONFIG_LOADED')) {
            return;
        }
        if (is_null($file)) {
            $file = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'config.inc';
        }
        if (file_exists($file)) {
            $data = file_get_contents($file);
            $config = eval($data);
            foreach ($config['ENV'] as $key => $value) {
                putenv("$key=$value");
            }
        }
        putenv('PDO_CASSANDRA_CONFIG_LOADED=1');
    }

    public static function skipIfNotLoaded()
    {
        if (!extension_loaded('pdo') or !extension_loaded('pdo_cassandra')) {
            die('skip PDO Cassandra driver not loaded.');
        }
    }

    public static function getDSN($host = 'localhost',
                                  $port = 9160,
                                  $keyspace = NULL)
    {
        $dsn = 'cassandra:';
        $dsn .= sprintf('host=%s', $host);
        if ($port) {
            $dsn .= sprintf(';port=%s', $port);
        }
        return $dsn;
    }
}
