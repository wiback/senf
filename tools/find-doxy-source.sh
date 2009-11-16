#!/bin/sh -e

base="`dirname "$0"`"

text="`echo "$1" | perl -pe 's/[^a-zA-Z0-9]+/ /g;s/^ //;s/ $//'`"
${base}/find-sources.sh -c \
    | xargs perl -0777ne '
          while(m{/\*\*<?(.*?)\*/|///<?(.*?)$}smg) {
              $t=$1||$2;
              $t=~s/\\\S+//g;
              $t=~s/[^a-zA-Z0-9]+/ /g;
              print $ARGV,"\n" if $t=~m{\Q'"$text"'\E};
          }'\
    | sort -u
