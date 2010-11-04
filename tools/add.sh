#!/bin/sh -e

if [ -z "$1" ]; then
    cat <<EOF
Usage: 
    $0 [-f] <name>.<ext> [<author>]
    $0 [-f] SConscript [<author>]

Will add the file with that name to the current directory by copying
the appropriate template and replacing the @NAME@ placeholders
appropriately.

<ext> needs to be one of

  hh   header
  ih   internal header
  cc   non-inline non-template code
  cci  inline non-template code
  ct   non-inline template code
  cti  inline template code
  mpp  special Boost.Preprocessor sourcefile (external iteration)
  dox  additional doxygen documentation file

If <author> is given, it is also set correctly replaced. If <author>
is unset but the environment variable SENF_AUTHOR is set, it's value
is used. Otherwise, the @AUTHOR@ placeholder will need to be replaced
manually.

$0 will refrain from overwriting existing files except when the '-f'
option is specified.
EOF
    exit 1
fi

base="`dirname "$0"`"; base="`cd "$base/_templates"; pwd`"
root="`cd "$base/../.."; pwd`"

if [ "$1" = "-f" ]; then
    force=1
    shift
fi

name="`basename "$1"`"
case "$name" in
    *.*) type=".${name#*.}"; name="${name%.$type}" ;;
    *) type=".$name" ;;
esac

if [ ! -r "$base/Example$type" ] ; then
    echo "Don't know about file type '$type'"
    exit 1
fi

[ -z "$2" ] || SENF_AUTHOR="$2"

if [ -z "$force" -a -r "$1" ] ; then
    echo "Target file '$1' exists."
    exit 1
fi

path="`dirname "$1"`"; path="`cd "$path"; pwd`"
path="$path/${name%%.*}"
path="${path#$root}"
path="$(echo "${path#/}" | tr / _)"

sed -e "s/@NAME@/$name/g" \
    -e "s/@PATH@/$path/g" \
    -e "s/@AUTHOR@/${SENF_AUTHOR:-@AUTHOR@}/g" \
    < "$base/Example$type" \
    > "$1"

svn add "$1"
svn propset svn:keywords "Author Date Id Revision" "$1"
