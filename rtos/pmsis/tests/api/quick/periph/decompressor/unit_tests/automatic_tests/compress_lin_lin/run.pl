#!/usr/bin/env perl

use strict;
use warnings;
use Data::Dumper;

print("== Testing Decompressor ==\n");

my $compressor_dir = "../test_utils/compressor";
my $timeout_cmd = "timeout --foreground -s KILL 600 ";

sub compile_compressor()
{
    my $res = system("mkdir -p $compressor_dir/build");
    if($res)
    {
        die("Creation of compressor build directory failed\n");
    }

    $res = system("cd $compressor_dir/build && cmake ..");
    if ($res)
    {
        die("Cmake build files generation failed\n");
    }

    $res = system("cmake --build $compressor_dir/build");
    if ($res)
    {
        die("Compilation of the compressor failed\n");
    }
}

sub generate_data_and_test
{
    my ($args) = @_;

    print("=== Generating data for test\n");
    my $SEED = 1234;

    my $mode = $args->{mode};
    my $signedness = "";
    if ($args->{unsigned})
    {
        $signedness = "-U";
    }
    my $datasize = $args->{datasize};
    my $qbit = $args->{qbit};
    my $start_bit = $args->{start_bit};
    my $start_byte = $args->{start_byte};

    my $centroids = $args->{centroids};
    my $centroids_args = "";
    if ($centroids > 0)
    {
        $centroids_args = "-C $centroids";
    }

    my $command = "$compressor_dir/build/compressor"
                  . " -s $SEED"
                  . " -b $start_bit"
                  . " -B $start_byte"
                  . " -E $mode"
                  . " $signedness"
                  . " -N 400"
                  . " -Q $qbit"
                  . " -S $datasize"
                  . " $centroids_args"
                  . " -O data.cgen";
    my $res = system($command);
    if($res)
    {
        printf("Failed command: $command\n");
        print("Failed configuration: ");
        print(Dumper($args));
        die("Data generation failed\n");
    }

    print "=== Executing a test...\n";
    $res = system "$timeout_cmd make all run platform=rtl";
    if ($res)
    {
        printf("Command to generate data: $command\n");
        print("Failed configuration: ");
        print(Dumper($args));
        die("Test Failed!\n");
    }
}

compile_compressor();

my $configuration_counter = 0;
my $mode = 0; # Compressions supports only T1 mode
for my $start_byte (0 .. 3)
{
    for my $start_bit ((0, 1, 4, 7))
    {
        for my $unsigned (0 .. 1)
        {
            for my $datasize ((1, 2, 4))
            {
                my $qbit_max = $datasize * 8 - 1;
                if ($qbit_max > 15)
                {
                    $qbit_max = 15;
                }
                for my $qbit ((1, 3, 5, 7, $qbit_max))
                {
                    $configuration_counter += 1;
                    generate_data_and_test({
                            mode => $mode,
                            unsigned => $unsigned,
                            datasize => $datasize,
                            qbit => $qbit,
                            centroids => 0,
                            start_bit => $start_bit,
                            start_byte => $start_byte,
                        });
                }
            }
        }
    }
}

print("Tested $configuration_counter configurations.\n");
