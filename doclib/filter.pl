#!/usr/bin/perl -n

if (/^\s*\\code\s*$/ .. /\\endcode/) {
    $i=length((/^(\s*)/)[0]) if /^\s*\\code\s*$/;
    print substr($_,$i);
} else {
    print;
}
