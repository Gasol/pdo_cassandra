<?php

class CassandraPDOTest
{
    const DEFAULT_CONFIG_FILE = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'config.inc';

    public static function factory($file = NULL)
    {
        self::loadConfig($file);

        $dsn = getenv('PDO_CASSANDRA_TEST_DSN');
        $username = getenv('PDO_CASSANDRA_TEST_USER');
        $password = getenv('PDO_CASSANDRA_TEST_PASS');

        if (!$db = new PDO($dsn, $username, $password)) {
            die("Could not create PDO object (DSN=$dsn, user=$username\n");
        }

        return $db;
    }

    protected static function loadConfig($file = NULL)
    {
        if (is_null($file)) {
            $file = DEFAULT_CONFIG_FILE;
        }
        if (file_exists($file)) {
            $data = file_get_contents($file);
            $config = eval($data);
            foreach ($config['ENV'] as $key => $value) {
                putenv("$key=$value");
            }
        }
    }

    public static function skipIfNotLoaded()
    {
        if (!extension_loaded('pdo') or !extension_loaded('pdo_cassandra')) {
            die('skip PDO Cassandra driver not loaded.');
        }
    }
}
