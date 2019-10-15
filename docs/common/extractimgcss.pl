#!/usr/bin/perl -w

use strict;

while (my $line = <STDIN>) {
    chomp($line);
    if ($line =~ m/!\[[^\]]+\]\(([^\)]+)\)\{ *width=([0-9]+px) *}/) {
        print "div.image img[src=\"".$1."\"] {\n" ;
        print "  width:".$2.";\n";
        print "}\n\n";
    }
}
