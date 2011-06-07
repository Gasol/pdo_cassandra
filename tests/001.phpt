--TEST--
Check for pdo_cassandra presence
--SKIPIF--
<?php if (!extension_loaded("pdo_cassandra")) print "skip"; ?>
--FILE--
<?php 
echo "pdo_cassandra extension is available";
/*
	you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the --EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure

	see php5/README.TESTING for further information on
  writing regression tests
*/
?>
--EXPECT--
pdo_cassandra extension is available
