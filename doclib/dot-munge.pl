#!/usr/bin/perl -i -n

# Reduce fontsize and change font
s/fontsize=10/fontsize=8/g; 
s/fontname="FreeSans.ttf"/fontname="Verdana"/g;

# Wrap long labels (templates and pathnames)
if (/label=\"([^"]{48,})\"/) {                              #"])){ # To make emacs happy ...
    $pre=$`; 
    $post=$';                                               #';    # To make emacs happy ...
    $label=$1;

    # Break at each komma or /
    $label=~s{[,/]}{$&\\r\\ \\ \\ \\ \\ \\ \\ \\ }g; 

    # If more than one '<' is in the label, break after each '<'
    if (($label=~tr/</</)>1) { 
        $label=~s/</<\\r\\ \\ \\ \\ \\ \\ \\ \\ /g;
    }

    # If at least one break is in there ...
    if ($label=~/\\r/) {
	# If it's a pathname, make all but the last line flush left
	# Otherwise only make first line flush left
	if ($label=~m{/}) {
	    $label=~s/\\r(\\ )*/\\ \\ \\ \\ \\ \\ \\ \\ \\l/g;
	    # Re-add blanks before last line
	    $label=~s/^.*\\l/$&\\ \\ \\ \\ \\ \\ \\ \\ /;
	} else {
	    $label=~s/\\r/\\ \\ \\ \\ \\ \\ \\ \\ \\l/;
	}
        # Make last line flush right
        $label.="\\r";
    }
    print "${pre}label=\"${label}\"${post}";
} else { 
    print;
}
