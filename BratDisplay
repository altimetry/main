#!/bin/sh

BRATDISPLAY=${0%BratDisplay}BratDisplay.app

if test ! -d "$BRATDISPLAY" ; then
  echo "ERROR: BratDisplay.app was not found at $BRATDISPLAY"
  echo "  Modify this script and adapt the BRATDISPLAY variable to point to the where BratDisplay.app is located"
  exit 1
fi

exec "$BRATDISPLAY/Contents/MacOS/BratDisplay" "$@"
