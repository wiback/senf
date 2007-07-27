#!/usr/bin/perl -i -n

# Reduce font size
s/fontsize=10/fontsize=8/g; 

# Wrap long labels (templates)
if (/label=\"([^"]*)\"/) {                                  #"])){ # To make emacs happy ...
    $pre=$`; 
    $post=$';                                               #';    # To make emacs happy ...
    $label=$1;

    # Break at each komma
    $label=~s/,/,\\r\\ \\ \\ \\ \\ \\ \\ \\ /g; 

    # If more than one '<' is in the label, break after each '<'
    if (($label=~tr/</</)>1) { 
        $label=~s/</<\\r\\ \\ \\ \\ \\ \\ \\ \\ /g;
    }

    # If at least one break is in there ...
    if ($label=~/\\r/) {
        # Make last line flush right
        $label.="\\r";
        # and first line flush left
        $label=~s/\\r/\\ \\ \\ \\ \\ \\ \\ \\ \\l/;
    }
    print "${pre}label=\"${label}\"${post}";
} else { 
    print;
}
