package gaptest::Loader;

use strict;
use warnings;
use base 'Exporter';

use YAML::Tiny;
use Carp;

our @EXPORT = qw(load_file);

=head1 NAME

gaptest::Loader - Loads gaptest.yml files

=head1 DESCRIPTION

Loads and verifies gaptest.yml files.

=cut

=item load_file

    my @test_variants = load_file "gaptest.yml";

Load gaptest test variants from a file.
Returns a list of test variants.

=cut

sub load_file {
    my ($file_name, undef) = @_;

    # Read the file
    my $file_content = YAML::Tiny->read($file_name);

    my %test_base = load_test_base($file_content);
    my @test_variants = load_test_variants ($file_content, \%test_base);

    return @test_variants;
}

sub load_test_base {
    my ($file_content, undef) = @_;
    my %test_base = ();

    if(defined $file_content->[0]->{name})
    {
        $test_base{name} = $file_content->[0]->{name};
    }
    else
    {
        croak "Test base name is not defined\n";
    }

    if(defined($file_content->[0]->{timeout}))
    {
        $test_base{timeout} = $file_content->[0]->{timeout};
    }
    else
    {
        $test_base{timeout} = 3600;
    }

    if(defined $file_content->[0]->{chips})
    {
        $test_base{chips} = [ @{$file_content->[0]->{chips}} ];
    }
    else
    {
        croak "No chip listed in the test base";
    }

    if(defined $file_content->[0]->{os})
    {
        $test_base{os} = [ @{$file_content->[0]->{os}} ];
    }
    else
    {
        croak "No OS listed in the test base";
    }

    if(defined $file_content->[0]->{platforms})
    {
        $test_base{platforms} = [ @{$file_content->[0]->{platforms}} ];
    }
    else
    {
        croak "No platform listed in the test base";
    }

    return %test_base;
}

sub load_test_variants {
    my ($file_content, $test_base_ref, undef) = @_;

    my @test_variants = ();

    if (defined $file_content->[0]->{variants})
    {
        my %variants = %{$file_content->[0]->{variants}};
        foreach my $section (sort keys %variants)
        {
            my $section_content = ${file_content}->[0]->{variants}->{$section};
            my %test_variant = load_test_variant ($section_content, $test_base_ref);
            push(@test_variants, \%test_variant);
        }
    }
    else
    {
        croak "No variant in test file\n";
    }

    return @test_variants;
}

sub load_test_variant {
    my ($section, $test_base_ref, undef) = @_;

    my %test_variant = ();

    my %test_base = %{$test_base_ref};

    # Load fields
    # use variant value if defined
    # else use test base value

    if(defined $section->{name})
    {
        $test_variant{name} = $test_base{name}."_".$section->{name};
    }
    else
    {
        croak "Test variant has no name";
    }

    if(defined $section->{flags})
    {
        $test_variant{flags} = $section->{flags};
    }
    else
    {
        $test_variant{flags} = "";
    }

    $test_variant{compile_only} = 0;
    if (defined $section->{compile_only})
    {
        if ($section->{compile_only} eq "true")
        {
            $test_variant{compile_only} = 1;
        }
    }

    if(defined $section->{duration})
    {
        $test_variant{duration} = $section->{duration};
    }
    else
    {
        croak "Test variant has no duration";
    }

    if(defined $section->{tags})
    {
        $test_variant{tags} = [ @{$section->{tags}} ];
    }
    else
    {
        croak "Test variant has no tag";
    }

    if(defined($section->{os}))
    {
        $test_variant{os} = [ @{$section->{os}} ];
    }
    else
    {
        $test_variant{os} = [ @{$test_base{os}} ];
    }

    if(defined($section->{chips}))
    {
        $test_variant{chips} = [ @{$section->{chips}} ];
    }
    else
    {
        $test_variant{chips} = [ @{$test_base{chips}} ];
    }

    if(defined($section->{platforms}))
    {
        $test_variant{platforms} = [ @{$section->{platforms}} ];
    }
    else
    {
        $test_variant{platforms} = [ @{$test_base{platforms}} ];
    }

    if(defined($section->{timeout}))
    {
        $test_variant{timeout} = $section->{timeout};
    }
    else
    {
        $test_variant{timeout} = $test_base{timeout};
    }

    return %test_variant;
}

1;
