#! /bin/sh
### BEGIN INIT INFO
# Provides:          #QtServiceName#
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: #QtServiceName# initscript
### END INIT INFO

# Use /usr/share/doc/initscripts-9.49.30/sysvinitfiles as reference for
# init.d scripts in this kind of system

# Do NOT "set -e"

NAME=#QtServiceName#
DESC=$NAME
DAEMON=#QtServiceFullPath#
USERNAME=#QtServiceUserName#
DAEMON_ARGS=
PIDFILE=/var/run/$NAME.pid
SCRIPTNAME=/etc/init.d/$NAME

# Exit if the package is not installed
[ -x "$DAEMON" ] || exit 0


. /etc/init.d/functions


do_start()
{
    daemon --user $USERNAME $DAEMON
    RETVAL=$?
    [ $RETVAL -eq 0 ] && touch $LOCKFILE
    echo
    return $RETVAL
}



do_stop()
{
    killproc $DAEMON
    RETVAL=$?
    [ $RETVAL -eq 0 ] && rm -f $LOCKFILE
    echo
    return $RETVAL
}



case "$1" in
  start)
    echo "Starting $DESC" "$NAME"
    do_start
	;;
  stop)
    echo "Stopping $DESC" "$NAME"
    do_stop
	;;
  status)
    status "$DAEMON" "$NAME" && exit 0 || exit $?
	;;
  *)
    echo "Usage: $SCRIPTNAME {start|stop|status}" >&2
	exit 3
	;;
esac

:

