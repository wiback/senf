#!/bin/sh

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
	-type f \( "$@" \)

