#!/usr/bin/perl -w
# strips width tags from images
# adds section numbers to all sections up to depth 4
# Copes with ^1^ and ~1~

use strict;

my %char_close = (
    "["  => "]",
    "`"  => "`",
);

my %char_start = (
    "^"  => "<SUP>",
    "~"  => "<SUB>",
);

my %char_end = (
    "^"  => "</SUP>",
    "~"  => "</SUB>",
);

sub mapline {
    my $line = shift(@_);
    my @res;
    my $inquotes;
    my @char_stack;
    my $prev = "";
    my $prevprev = "";
    for my $i (0..length($line)-1) {
        my $c = substr($line, $i, 1);
        if ($inquotes) {
            push @res, $c;
            if (($prev ne "\\" || $prevprev eq "\\") && $c eq $inquotes) {
                $inquotes = "";
            }
        } elsif (scalar @char_stack gt 0) {
            if ($c eq $char_stack[scalar @char_stack - 1]) {
                my $char = pop(@char_stack);
                push @res, $char_end{$char};
            } else {
                push @res, $c;
            }
        } elsif (($prev ne "\\" || $prevprev eq "\\") && defined $char_start{$c}) {
            push @char_stack, $c;
            push @res, $char_start{$c};
        } elsif (($prev ne "\\" || $prevprev eq "\\") && defined $char_close{$c}) {
            $inquotes = $char_close{$c};
            push @res, $c;
        } else {
            push @res, $c;
        }
        $prevprev = $prev;
        $prev = $c;
    }
    return join '', @res;
}

my $i = 0;
my $skip = 0;
while (my $line = <STDIN>) {
    chomp($line);
    if ($line =~ m/^\~{2,5}/) {
    	$skip = not $skip;
    	print "$line\n";
    	next;
    } elsif ($skip) {
    	print "$line\n";
    	next;
    }
    # Call the mapper first
    $line = mapline($line);
    # Remove image widths
    if ($line =~ m/(!\[[^\]]+\]\([^\)]+\))\{ *width=[0-9]+px *}/) {
        print "$1\n" ;
    # add {#sectionn} tag to sections 1-4 deep
    } elsif ($line =~ m/^(\s*#{1,4} .*$)/) {
        print "$line {#section".$i."}\n" ;
        $i = $i + 1 ;
    # Remove /newpage's
    } elsif ($line !~ m/^\\newpage/) {
        print "$line\n" ;
    }
}
