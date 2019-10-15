#!/usr/bin/perl -w

use strict;

while (my $line = <STDIN>) {
    chomp($line);
    if ($line =~ m/{ TABS HERE }/) {
		foreach my $argnum (0 .. $#ARGV) {
		   print "arr.push(new tabElement(\"$ARGV[$argnum]\", \"/$ARGV[$argnum]/html/\", \"$ARGV[$argnum]\"));\n";
		}
    } else {
    	print "$line\n"
    }
}
