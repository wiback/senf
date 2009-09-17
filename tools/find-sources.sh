#!/bin/sh -e

if [ -z "$1" ]; then
    echo "Usage:"
    echo "  $0 [-c] <commands>"
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
    cond='( -name *.h -o -name *.hh -o -name *.ih -o -name *.c -o -name *.cc -o -name *.cci -o -name *.ct -o -name *.cti -o -name *.mpp -o -name *.dox )'
fi

set -f

find . \
    -name .svn -prune -o \
    -name .git -prune -o \
    -name doc -prune -o \
    -name debian -prune -o \
    -name dist -prune -o \
    -name build -prune -o \
    -name "*.a" -o \
    -name "*.o" -o \
    -name "*~" -o \
    -name "#*#" -o \
    -name "*.pyc" -o \
    -name .test.bin -o \
    -name "*.stamp" -o \
    -name ".sconsign*" -o \
    -name "semantic.cache" -o \
    -name "all_includes.hh" -o \
    -type f $cond \( "$@" \)
