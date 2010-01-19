#!/bin/sh

if [ `whoami` != root ]; then
    echo "Must be root."
    exit 2
fi

base="`dirname "$0"`"; base="`cd "$base"; pwd`"

aptitude update
aptitude install grep-dctrl

packages="$(grep-dctrl -ns Build-Depends senf <"$base/control" \
	| tr ',\n\t' '   '  \
	| sed -e 's/([^)]*)//g' -e 's/ \+/ /g' -e 's/ \+| \+[^ ]\+//g' -e 's/ $//')"

aptitude install $packages
