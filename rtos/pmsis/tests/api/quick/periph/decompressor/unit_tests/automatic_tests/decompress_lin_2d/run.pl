#!/usr/bin/env perl

use strict;
use warnings;
use Data::Dumper;

print("== Testing Decompressor (L2 1D => L1 2D) ==\n");
my $timeout_cmd = "timeout --foreground -s KILL 600 ";

sub generate_data_and_test
{
    my ($args) = @_;

    print("=== Generating data for test\n");

    my $signedness = "";
    if ($args->{unsigned})
    {
        $signedness = "0";
    }
    else
    {
        $signedness = "1";
    }
    my $datasize = $args->{datasize};
    my $qbit = $args->{qbit};

    my $item_count = $args->{item_count};
    my $stride_length = $args->{stride_length};
    my $transfer_size = $args->{transfer_size};

    my $start_bit = $args->{start_bit};
    my $start_byte = $args->{start_byte};

    my $special_symbol = 1234;

    # compute needed number of L2 elements using transfer size and stride_length
    # times 2 to be safe
    my $memory_elements = $transfer_size * $stride_length * 2;

    my $command = "python3 decomp_L2_1D_L1_2D.py --transfer_size $transfer_size"
                                             . " --use_sign $signedness"
                                             . " --qbit $qbit"
                                             . " --word_extension $datasize"
                                             . " --item_count $item_count"
                                             . " --stride_length $stride_length"
                                             . " --special_symbol $special_symbol"
                                             . " --start_bit $start_bit"
                                             . " --start_byte $start_byte"
                                             . " --L2_memory_elements $memory_elements";

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

my $configuration_counter = 0;
for my $unsigned (0 .. 1)
{
    for my $start_byte (0 .. 3)
    {
        for my $start_bit ((0, 1, 4, 7))
        {
            for my $stride_length ((4, 6, 10, 20))
            {
                for my $item_count ((1, 2, 3, $stride_length - 1))
                {
                    for my $datasize ((0, 1, 3))
                    {
                        my $qbit_max = ($datasize+1) * 8 - 1;
                        if ($qbit_max > 15)
                        {
                            $qbit_max = 15;
                        }
                        for my $qbit ((1, 3, 5, 7, $qbit_max))
                        {
                            $configuration_counter += 1;
                            generate_data_and_test({
                                    unsigned => $unsigned,
                                    datasize => $datasize,
                                    qbit => $qbit,
                                    transfer_size => 100,
                                    item_count => $item_count,
                                    stride_length => $stride_length,
                                    start_bit => $start_bit,
                                    start_byte => $start_byte,
                                });
                        }
                    }
                }
            }
        }
    }
}

print("Tested $configuration_counter configurations.\n");
