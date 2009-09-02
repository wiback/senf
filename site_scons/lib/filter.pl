#!/usr/bin/perl -n

BEGIN {
    ($libdir=$0) =~ s{/[^/]*$}{};
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
elsif (/\\diaimage\s+(\S+)/) {
    $dia=$1;
    ($png=$dia)=~s/\.dia$/.png/;
    system("python $libdir/makeDiaImageMap.py $dia");
}
else {
    print $_,"\n";
}
