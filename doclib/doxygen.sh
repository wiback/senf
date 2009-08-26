#!/bin/bash -e

do_html_cleanup()
{
    sed -e 's/id="current"/class="current"/' \
	| tidy -ascii -q --wrap 0 --show-warnings no --fix-uri no \
	| sed -e 's/name="\([^"]*\)"\([^>]*\) id="\1"/name="\1"\2/g' \
	| xsltproc --novalid --nonet --html --stringparam topdir "$reltopdir" \
	      "$base/html-munge.xsl" -
}

###########################################################################

# Build absolute, normalized pathname
abspath()
{
    case "$1" in
	/*) echo "$1" ;;
	*) echo "`pwd`/$1" ;;
    esac | sed \
	-e 's/\/\//\//g' \
	-e:a \
	-e 's/\/\.\(\/\|$\)/\1/g' \
	-eta \
	-e:b \
	-e 's/\/[^\/]*\/..\(\/\|$\)/\1/g' \
	-etb
}

# Create relative path from absolute directory $1 to absolute path $2
relpath()
{
    local src="${1#/}"
    local dst="${2#/}"
    while true; do
	if [ -z "$src" -a -z "$dst" ]; then
	    echo "Internal error in relpath()" 1>&2
	    exit 1
	fi
	srcd="${src%%/*}"
	dstd="${dst%%/*}"
	if [ "$srcd" = "$dstd" ]; then
	    src="${src#$srcd}"; src="${src#/}"
	    dst="${dst#$dstd}"; dst="${dst#/}"
	else
	    break
	fi
    done
    rel="`echo "$src" | sed -e "s/[^\/]*/../g"`/$dst" # `"
    echo "${rel%/}"
}

# Log executed commands
cmd()
{
    echo "+" "$@"
    "$@"
}

html_cleanup()
{
    mv "$1" "${1}.orig"
    do_html_cleanup <"${1}.orig" >"$1"
}

###########################################################################

name="`basename "$0"`"; #`"
base="`dirname "$0"`"; base="`cd "$base"; pwd`" #`"

TEMP=`getopt -n $name -o '' -l html,tagfile,tagfile-name:,tagfiles:,output-dir:,html-dir:  -- "$@"`
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
eval set -- "$TEMP"

tagxsl="$base/tag-munge.xsl"

html="NO"; tagfile="NO"; tagfile_name=""; tagfiles=""; output_dir="doc"; html_dir="html"

while true; do
    case "$1" in
	--html) 
	    html="YES" ; shift ;;
	--tagfile) 
	    tagfile="YES" ; shift ;;
	--tagfile-name) 
	    tagfile_name="$2"; shift 2 ;;
	--tagfiles) 
	    for f in $2; do
		f="`abspath "$f"`" #`"
		tagfiles="$tagfiles${tagfiles:+ }$f"
	    done
	    shift 2
	    ;;
	--output-dir)
	    output_dir="$2"; shift 2 ;;
	--html-dir)
	    html_dir="$2"; shift 2 ;;
	--)
	    shift; break ;;
	*)
	    echo "Internal error: $*"; exit 1 ;;
    esac
done

if [ -z "$1" ]; then
    cat <<EOF
Usage:
    $name
        [--html]
        [--tagfile]
        [--tagfile-name=<name>]
        [--tagfiles=<files>]
        [--output-dir=<dir>]
        [--html-dir=<dir>]
            <doxyfile-or-dir>
EOF
    exit 1
fi

if [ "$tagfile" = "YES" -a -z "$tagfile_name" ]; then
    echo "--tagfile-name is required with --tagfile"
    exit 1
fi

doxydir="$1"

if [ -f "$doxydir" ]; then
    doxydir="`dirname "$doxydir"`" #`"
fi

doxydir="`abspath "$doxydir"`" #`"

###########################################################################

## Find $TOPDIR

cd "$doxydir"
while [ ! -r "SConstruct" -a "`pwd`" != "/" ]; do cd ..; done
if [ ! -r "SConstruct" ]; then
    echo "topdir not found"
    exit 1;
fi
TOPDIR="`pwd`";
reltopdir="`relpath "$doxydir/$output_dir/$html_dir" "$TOPDIR"`" #`"
cd "$doxydir"


## Remove tagfile_name from list of tagfiles

if [ -n "$tagfile_name" ]; then 
    tagfile_name="`abspath "$output_dir/$tagfile_name"`" #`"
    x="$tagfiles"; tagfiles=""
    for f in $x; do
	if [ "$f" != "$tagfile_name" ]; then
	    tagfiles="$tagfiles${tagfiles:+ }$f"
	fi
    done
fi

## Call doxygen proper

generate_tagfile=""
if [ "$tagfile" = "YES" ]; then
    generate_tagfile="$tagfile_name"
fi
export TOPDIR html tagfile tagfile_name tagfiles output_dir html_dir generate_tagfile

cmd ${DOXYGEN:-doxygen}


## Clean up tagfile, if generated

if [ "$tagfile" = "YES" ]; then
    mv "$tagfile_name" "${tagfile_name}.orig"
    cmd xsltproc --novalid --nonet -o "$tagfile_name" "$tagxsl" "${tagfile_name}.orig"
fi


## Call installdox

if [ -n "$tagfiles" ]; then
    args=""
    for f in $tagfiles; do
	n="`basename "$f"`" #`"
	d="`dirname "$f"`" #`"
	url="`relpath "$doxydir/$output_dir/$html_dir" "$d/$html_dir"`" #`"
	args="$args${args:+ }-l $n@$url"
    done

    (
	cd "./$output_dir/$html_dir"
	cmd "./installdox" $args
    )
fi


## Postprocess html files, if generated

if [ "$html" = "YES" ]; then
    for h in "$doxydir/$output_dir/$html_dir"/*.html; do
	cmd html_cleanup "$h"
    done
fi
