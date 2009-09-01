#!/usr/bin/perl -n

BEGIN {
    ($topdir=$0) =~ s{doclib/.*$}{};
    print $topdir,"\n";
}

s/\s*$//;
while (s/\t/' 'x(8-length($`)%8)/e) {}

if (/^\s*\\code$/ .. /\\endcode/ && !/^$/) {
    $i=length($1) if /^(\s*)\\code$/;
    print substr($_,$i),"\n";
} 
elsif (s/^(\s*)<pre>$/$1<pre class="fragment">/ .. /<\/pre>/ && !/^$/) {
    $i=length($1) if /^(\s*)<pre class="fragment">$/;
    print substr($_,$i),"\n";
} 
else {
    print $_,"\n";
}
