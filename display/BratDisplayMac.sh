#!/bin/sh

BRATGUIDIR=/Applications

if test ! -d "$BRATGUIDIR/BratGui.app" ; then
  echo "ERROR: BratGui was not found at $BRATGUIDIR"
  echo "  Modify this script and adapt the BRATGUIDIR variable to point to the directory where BratGui is located"
  exit 1
fi

BRATDISPLAY="$BRATGUIDIR/BratGui.app/Contents/MacOS/BratDisplay"

if test ! -f "$BRATDISPLAY" ; then
  echo "ERROR: BratDisplay executable was not found at $BRATDISPLAY"
  echo "  Is your BratGui installation correct?"
  exit 1
fi

exec "$BRATDISPLAY" "$@"
