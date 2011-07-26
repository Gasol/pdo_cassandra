#
# Regular cron jobs for the php5-cassandra package
#
0 4	* * *	root	[ -x /usr/bin/php5-cassandra_maintenance ] && /usr/bin/php5-cassandra_maintenance
