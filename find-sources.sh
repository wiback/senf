#!/bin/sh -e

if [ -z "$1" ]; then
    echo "Usage:"
    echo "	$0 [-c] <commands>"
    echo
    echo "-c Only list C/C++ source files"
    echo
    echo "<commands> are any find commands (which should contain a -print somewhere)"
    echo "which are matched for any file fond"
    exit 1
fi

cond=""
if [ "$1" = "-c" ]; then
    shift
    cond='( -name *.h -o -name *.hh -o -name *.ih -o -name *.c -o -name *.cc -o -name *.cci -o -name *.ct -o -name *.cti )'
fi

find . \
	-name .svn -prune -o \
	-name doc -prune -o \
	-name "*.a" -o \
	-name "*.o" -o \
	-name "*~" -o \
	-name "#*#" -o \
	-name "*.pyc" -o \
	-name .test.bin -o \
	-name "*.stamp" -o \
	-name ".sconsign*" -o \
    -type f $cond \( "$@" \)

