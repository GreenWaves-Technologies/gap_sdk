import sys
import getopt
import random
import ctypes
import argparse

# Creates a golden model that decompresses data from a 1D L2 memory
# to an L1 2D memory

def parse_arguments(parser):
    parser.add_argument('--transfer_size', type=int, help="Total items to transfer: value between 1 to 4096")
    parser.add_argument('--use_sign', type=int, help="Use a sign extension: 0 or 1")
    parser.add_argument('--qbit', type=int, help="Number of quantization bits: Value between 1 to 16")
    parser.add_argument('--word_extension', type=int, help="Word extension: 0: 8-bit (byte), 1: 16-bit (half-word), 3: 32-bit (word)")
    parser.add_argument('--item_count', type=int, help="Number of items to decompress/compress (T1) during the linear part of the 2D transfer")
    parser.add_argument('--stride_length', type=int, help="Number of items to JUMP (T1) during the linear part of the 2D transfer to move to the second chunk")
    parser.add_argument('--special_symbol', type=int, help="Symbol used for T3 transfers mostly (used here only for coverage)")
    parser.add_argument('--L2_memory_elements', type=int, help="Number of elements stored in L2 cache memory")
    parser.add_argument('--start_bit', type=int, help="Starting bit")
    parser.add_argument('--start_byte', type=int, help="Starting byte")

    args = parser.parse_args()

    if (args.transfer_size == None or args.transfer_size < 1 or args.transfer_size > 4096):
        raise argparse.ArgumentError(args.transfer_size, "Value must be between 1 and 4096")
    if (args.use_sign < 0 or args.use_sign > 1):
        raise argparse.ArgumentError(args.use_sign, "Can be only 0 (unsigned) or 1 (signed)")
    if (args.qbit < 1 or args.qbit > 16):
        raise argparse.ArgumentError(args.qbit, "qbit must be between 1 and 16")
    if (args.word_extension == '0' or args.word_extension == '1' or args.word_extension == '3'):
        raise argparse.ArgumentError(args.word_extension, "Word extension must be either 0 (8 bits), 1 (16 bits) or 3 (32 bits)")
    if (args.item_count > args.stride_length):
        raise argparse.ArgumentError(args.stride_length, "Stride length must be higher than the item count")
    if (args.L2_memory_elements == None or args.L2_memory_elements < 1 or args.L2_memory_elements > 4096):
        raise argparse.ArgumentError(args.L2_memory_elements, "L2 memory elements must be between 1 and 4096")
    if (args.start_bit < 0 or args.start_bit > 8):
        raise argparse.ArgumentError(args.start_bit, "Start bit should be between 0 and 7")
    if (args.start_byte < 0 or args.start_byte > 3):
        raise argparse.ArgumentError(args.start_byte, "Start byte should be between 0 and 3")

def output_to_file():
    # Output golden model of L2 2D data decompressed into L1 to a file
    file = open("compressed_data.c", "w")

    file.write('/******************\n')
    file.write(' * GENERATED FILE *\n')
    file.write(' * DO NOT COMMIT !*\n')
    file.write('*******************/\n\n')

    file.write('/* -------- */\n')
    file.write('/* includes */\n')
    file.write('/* -------- */\n')
    file.write('#include <stdint.h>\n\n')

    word_size_table = ['int8_t', 'int16_t', 'int32_t']
    word_size = word_size_table[word_length_choice-1]

    if (word_length_choice == 0):
        word_size = 'int8_t'
    elif (word_length_choice == 1):
        word_size = 'int16_t'
    elif (word_length_choice == 3):
        word_size = 'int32_t'
    else:
        raise RuntimeError("invalid word size");

    if (use_sign):
        data_type = ""
    else:
        data_type = "u"

    hex_zero_fill_table = [2, 4, 8]

    hzero_fill = hex_zero_fill_table[word_length_choice-1]

    # Write the golden model decompressed elements
    file.write('/* ------------------------------------------ */\n')
    file.write('/* expected decompressed data in cluster TCDM */\n')
    file.write('/* ------------------------------------------ */\n')
    file.write('static PI_L1 u' + word_size + ' tcdm_expect_data[' + str(list_length) + '] = {\n')

    # Decompress in byte size
    if (word_length_choice == 0):
        for i in range(0, list_length):
            byte = decompressed_list[i]
            byte = byte[6:]

            if (use_sign == 1):
                # Turn hex to binary
                byte = bin(int(byte, 16))[2:].zfill(8)

                # If MSB is '1' then pad the rest of bits with '1's
                if (byte[8-qbit] == '1'):
                    # If qbit > than a byte then just write the byte and discard
                    # the excess qbits
                    if (qbit > 8):
                        byte = ('1' * 24 + byte)
                    else:
                        # Pad with '1's if the number is negative
                        byte = ('1' * (8-qbit)) + byte[8-qbit:]
                # Else just turn back to hex and continue
                byte = '{:02x}'.format(int(byte, 2))

            byte = int(byte, 16)
            byte = hex(byte)[2:].zfill(2)
            file.write("    " + "0x" + byte + ", // " + "@" + str(i) + "\n")

        file.write("")
        file.write("};\n\n\n\n\n")

    # Decompress in half word size
    if (word_length_choice == 1):
        for i in range(0, list_length):
            half_word = decompressed_list[i]
            half_word = half_word[4:]

            if (use_sign == 1):
                # Turn hex to binary
                half_word = bin(int(half_word, 16))[2:].zfill(16)

                # If MSB is '1' then pad the rest of bits with '1's
                if (half_word[16-qbit] == '1'):
                    # Pad with '1's if the number is negative
                    half_word = ('1' * (16-qbit)) + half_word[16-qbit:]
                # Else just turn back to hex and continue
                half_word = '{:04x}'.format(int(half_word, 2))

            half_word = int(half_word, 16)
            half_word = hex(half_word)[2:].zfill(2)
            file.write("    " + "0x" + half_word + ", // " + "@" + str(i) + "\n")

        file.write("")
        file.write("};\n\n\n\n\n")

    # Decompress in word size
    if (word_length_choice == 3):
        for i in range(0, list_length):
            word = decompressed_list[i]

            if (use_sign == 1):
                # Turn hex to binary
                word = bin(int(word, 16))[2:].zfill(32)

                # If MSB is '1' then pad the rest of bits with '1's
                if (word[32-qbit] == '1'):
                    # Pad with '1's if the number is negative
                    word = ('1' * (32-qbit)) + word[32-qbit:]
                # Else just turn back to hex and continue
                word = '{:08x}'.format(int(word, 2))

            word = int(word, 16)
            word = hex(word)[2:].zfill(hzero_fill)
            file.write("    " + "0x" + word + ", // " + "@" + str(i) + "\n")
        file.write("")
        file.write("};\n\n\n\n\n")

    # Write the random data to load to the L2 memory
    file.write('/* ------------------------------------- */\n')
    file.write('/* L2 Data to decompress in cluster TCDM */\n')
    file.write('/* ------------------------------------- */\n')
    file.write('static PI_L2 uint32_t l2_compr_data[' + str(total_L2_memory_elements) + '] = {\n')
    for i, comp_item in enumerate(L2_memory):
        comp_item = hex(comp_item)
        file.write("    " + comp_item + ", // " + "@" + str(i) + "\n")
    file.write("};\n")

    # Writes the configuration for the register
    file.write("/* ------------------------------ */\n")
    file.write("/* decompressor command structure */\n")
    file.write("/* ------------------------------ */\n")

    file.write("pi_cl_dma_decompressor_cmd_t cmd = {\n")
    file.write("    .ext                    = (uint32_t) l2_compr_data,\n"),
    file.write("    .loc                    = (uint32_t) NULL, // should be set before testing\n"),
    file.write("    .conf.items_to_transfer = {},\n".format(transfer_size)),
    file.write("    .conf.decompr_mode      = PI_CL_DMA_DECOMPRESSOR_MODE_T1,\n"),
    file.write("    .conf.decompr_direction = PI_CL_DMA_DECOMPRESSOR_DIR_EXT2LOC,\n"),
    file.write("    .transf_type            = PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_2D,\n"),
    file.write("    .conf.extension_type    = PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_{},\n"
               .format(8 * (word_length_choice+1))),
    file.write("    .conf.sign_extension = PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_{},\n"
               .format("SIGNED" if use_sign else "UNSIGNED")),
    file.write("    .conf.item_bit_width    = {},\n".format(qbit)),
    file.write("    .conf.start_bit         = {},\n".format(start_bit)),
    file.write("    .conf.start_byte        = {},\n".format(start_byte)),
    file.write("    .lut                    = NULL,\n"),
    file.write("    .lut_size               = 0,\n"),
    file.write("    .l2_2d.count            = 0,\n"),
    file.write("    .l2_2d.stride           = 0,\n"),
    file.write("    .tcdm_2d.count          = {},\n".format(item_count - 1)),
    file.write("    .tcdm_2d.stride         = {},\n".format(stride_length - 1)),
    file.write("    .special_symbol         = {},\n".format(special_symbol)),
    file.write("    .done                   = 0,\n"),
    file.write("    .flag.update_lut        = 0,\n"),
    file.write("    .flag.update_2d_ext     = 0,\n"),
    file.write("    .flag.update_2d_loc     = 1,\n"),
    file.write("};\n\n")

    file.close()

# Parse the script arguments
parser = argparse.ArgumentParser()
parse_arguments(parser)
args = parser.parse_args()

# If arguments are ok assign them to the values
use_sign = args.use_sign
transfer_size = args.transfer_size
qbit = args.qbit
item_count = args.item_count
stride_length = args.stride_length
total_L2_memory_elements = args.L2_memory_elements
start_bit = args.start_bit
start_byte = args.start_byte
# 0: 8 bits, 1: 16bits, 3: 32 bits
word_length_choice = args.word_extension
special_symbol = args.special_symbol

word_length = 2**32

hex_zero_fill_table = [2, 4, 8]
bin_zero_fill_table = [8, 16, 32]

hex_zero_fill = 32
bin_zero_fill = 32



# Create a list with N numbers in range (0, 2**N) and put them in L2 memory as compressed data
randomlist = random.sample(range(0, word_length), total_L2_memory_elements)

L2_memory = randomlist[:]


# Fill hex numbers with zeroes if needed
for i in range(0, total_L2_memory_elements):
    randomlist[i] = '{:08x}'.format(randomlist[i]).zfill(hex_zero_fill)


item_index = 0
item_to_decomp = randomlist[item_index]


item_to_decomp = bin(int(item_to_decomp, 16))[2:].zfill(bin_zero_fill)


start_bit_index = (args.start_byte * 8) + args.start_bit
end_bit_index = start_bit_index + qbit

# Items that are decompressed from L2 memory to the L1
decompressed_list = []


# Indexes from where in the array to read 'item_count' elements
stride_index = 0

elements_remaining = transfer_size

# Engine to generate the decompressed 2D data to L1
while (elements_remaining > 0):
    # Start from stride index and read item_count items
    for decomp_item in range(0, item_count):
        # If qbit word spans between 2 L2 elements then we read
        # from N bit up to 31 and
        # from 0 of the next L2 element up to the R bits remaining
        # Crossword schematic example:
        #     L2 element N   [ --------(---- ],
        #     L2 element N+1 [ --)---------- ]
        if (elements_remaining <= 0):
            # no more element to compress
            decompressed_list.append("0x00000000")
            continue
        if (end_bit_index) > 32:
            cross_word_start = item_to_decomp[0:32-start_bit_index]

            item_index += 1
            item_to_decomp = randomlist[item_index]
            item_to_decomp = bin(int(item_to_decomp, 16))[2:].zfill(bin_zero_fill)

            cross_word_end = item_to_decomp[32-(end_bit_index-32):32]

            start_bit_index = end_bit_index - 32
            end_bit_index = start_bit_index + qbit


            cross_word = cross_word_end + cross_word_start
            decompressed_list.append('{:08x}'.format(int(cross_word, 2)).zfill(8))
            elements_remaining -= 1
            continue

        # Use the binary string and select specific bits to decompress. Then turn to hex and add leading zeros
        item_to_decomp = randomlist[item_index]
        item_to_decomp = bin(int(item_to_decomp, 16))[2:].zfill(bin_zero_fill)
        item = item_to_decomp[32-end_bit_index:32-start_bit_index]



        decompressed_list.append('{:08x}'.format(int(item, 2)).zfill(8))
        start_bit_index += qbit
        end_bit_index += qbit
        elements_remaining -= 1


    # After we finish the decompressing of the linear part of a 2D array
    # we jump to the next stride offset and continue for the next items.
    # The items jumped are assumed to be 0 when testing
    for i in range(0, stride_length-item_count):
        decompressed_list.append("0x00000000")

    item_to_decomp = randomlist[item_index]
    item_to_decomp = bin(int(item_to_decomp, 16))[2:].zfill(bin_zero_fill)

list_length = len(decompressed_list)

output_to_file()
