#!/bin/sh

cd "`dirname "$0"`"

trap 'rm -f installed_includes source_includes' 0 1 2 15

scons -n install_all PREFIX="non-existing-target" \
    | awk -F \" '/^Install file:/ && $2~/\.(h|hh|ih|ct|cti|cci|mpp)$/ {print $2}' \
    | sort > installed_includes

./find-sources.sh -c ! -name "*.cc" ! -path "./debian/*" ! -name "*.test.*" -printf "%P\n" \
    | sort > source_includes

comm -13 installed_includes source_includes

