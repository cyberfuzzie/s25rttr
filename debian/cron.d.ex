#
# Regular cron jobs for the s25rttr package
#
0 4	* * *	root	[ -x /usr/bin/s25rttr_maintenance ] && /usr/bin/s25rttr_maintenance
