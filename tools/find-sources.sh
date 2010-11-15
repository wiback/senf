#!/bin/sh -e

print="-print"
cond=""
while true ; do
    case "$1" in
        -n) print=""; shift ;;
        -c) cond='( -name *.h -o -name *.hh -o -name *.ih -o -name *.c -o -name *.cc -o -name *.cci -o -name *.ct -o -name *.cti -o -name *.mpp -o -name *.dox )'; shift ;;
        *) break ;;
    esac
done

set -f

find . \
    -name .svn -prune -o \
    -name .git -prune -o \
    -name .sconf_temp -prune -o \
    -name doc -prune -o \
    -name debian -prune -o \
    -name dist -prune -o \
    -name build -prune -o \
    -path "*/tools/scons-*" -prune -o \
    -name "*.a" -o \
    -name "*.o" -o \
    -name "*.os" -o \
    -name "*.so" -o \
    \( -type f -a ! -name "*.*" -a -exec sh -c "file --brief {} | grep -q ELF" \; \) -o \
    -name "*~" -o \
    -name "#*#" -o \
    -name "*.pyc" -o \
    -name .test.bin -o \
    -name "*.stamp" -o \
    -name ".sconsign*" -o \
    -name "semantic.cache" -o \
    -name "all_includes.hh" -o \
    -type f $cond \( "$@" $print \)
