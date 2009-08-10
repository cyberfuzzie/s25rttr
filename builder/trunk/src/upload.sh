#!/bin/bash

. base.inc.sh

[ -z "$ARCH" ] && usage

CURRENT=$(reprepro -Vb /www/ra-doersch.de/nightly/s25client/deb list sid s25rttr | grep $ARCH | cut -f 3 -d ' ')
NEW=$VERSION-$REVISION

exit 1
