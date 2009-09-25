#!/bin/sh -e

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
    -type f $cond \( "$@" -print \)
