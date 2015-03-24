#!/bin/sh

if [ -z "$1" ]; then
    echo "Usage: $0 <xml file> [node options...]"
    exit 1
fi

base="`dirname "$0"`"; base="`cd "$base"; pwd`"
srcdir="`dirname "$1"`"; srcdir="`cd "$srcdir"; pwd`"
src="$srcdir/`basename "$1"`"
shift

if [ ! -r "$src" ]; then
    echo "failed to read $src"
    exit 1
fi

tmpdir="/tmp/xmlrun-$$"
pids=""
trap 'rm -rf $tmpdir; [ -z "$pids" ] || kill $pids' 0 1 2 15

mkdir -p "$tmpdir"

cd "$tmpdir"
xsltproc "$base/parsexml.xslt" "$src"
for nodeconf in node-*.conf; do
    "$base/../carmen" --config="channels.conf" --config="coordinates.conf" --config="$nodeconf" "$@" &
    pids="$pids${pids:+ }$!"
done

wait

pids=""