/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Mathieu Barbe, GreenWaves Technologies (mathieu.barbe@greenwaves-technologies.com)
 */

#include "stdio.h"
#include "string.h"
#include "stdint.h"

#include "pmsis.h"
#include "pmsis/rtos/pmsis_assert.h"

#include "bsp/partition.h"
#include "bsp/flash/hyperflash.h"

void test_bad_addr(pi_device_t *part)
{
    size_t part_size;
    uint8_t *data;

    puts("\nTest partition bad address");

    part_size = pi_partition_get_size(part);

    pi_assert(
            data = pi_l2_malloc(8));

    // First bad address
    pi_assert(
            pi_partition_read(part, part_size, data, 1) < 0);

    // Last authorized address
    pi_assert(
            pi_partition_erase(part, part_size - 8, 8) >= 0);
    pi_assert(
            pi_partition_write(part, part_size - 8, data, 8) >= 0);

    // First bad address in case of erase sector
    pi_assert(
            pi_partition_erase(part, part_size, 8) < 0);

    pi_l2_free(data, 8);

    puts("Test partition bad adress done");
}

void test_erase_partition(pi_device_t *part)
{
    uint8_t *data;
    const size_t data_size = 128;
    const uint32_t addr = 0x80000;

    puts("\nTest erase partition");

    pi_assert(
            data = pi_l2_malloc(data_size));

    // Init flash area
    pi_assert(
            pi_partition_erase(part, addr, data_size) >= 0);

    printf("Write and check data on flash at 0x%lx\n", addr);
    memset(data, 200, data_size);
    pi_assert(
            pi_partition_write(part, addr, data, data_size) >= 0);
    pi_assert(
            pi_partition_read(part, addr, data, data_size) >= 0);
    for (size_t i = 0; i < data_size; i++)
    {
        pi_assert(data[i] == 200);
    }

    // Erase sector
    puts("Erase data");
    pi_assert(
            pi_partition_erase(part, addr, data_size) >= 0);

    puts("Check if data was deleted");
    pi_assert(
            pi_partition_read(part, addr, data, data_size) >= 0);
    for (size_t i = 0; i < data_size; i++)
    {
        pi_assert(data[i] != 200);
    }

    pi_l2_free(data, data_size);

    puts("Test erase done");
}

void test_write_partition(pi_device_t *part)
{
    uint8_t *data;
    uint8_t *data2;
    const uint32_t addr = 0x40000;

    puts("\nTest write partition");

    pi_assert(
            (data = pi_l2_malloc(8)));
    pi_assert(
            (data2 = pi_l2_malloc(8)));

    // Initialize data
    printf("data: ");
    for (uint8_t i = 0; i < 8; i++)
    {
        data[i] = i;
        printf("%u ", data[i]);
    }
    puts("");

    // Erase sector
    printf("Erase sector at 0x%lx\n", addr);
    pi_assert(
            pi_partition_erase(part, addr, 8) >= 0);

    // Write data
    printf("Write data at 0x%lx\n", addr);
    pi_assert(
            pi_partition_write(part, addr, data, 8) >= 0);

    // Read partition for check writing operation
    puts("Reads the data written and check their integrity");
    pi_assert(
            pi_partition_read(part, addr, data2, 8) >= 0);

    for (uint8_t i = 0; i < 8; i++)
    {
        pi_assert(data[i] == data2[i]);
    }

    pi_l2_free(data, 8);
    pi_l2_free(data2, 8);

    puts("Test write artition done");
}

void test_read_partition(pi_device_t *part)
{
    int8_t rc;
    char *buff;
    const size_t buff_size = 16;
    const char *pattern = "FS in partition";

    printf("\nTest read partition 1\n"
           "Check pattern `%s' at 0x\n", pattern);

    pi_assert(
            buff = pi_l2_malloc(buff_size));
    pi_assert(
            pi_partition_read(part, 0, buff, buff_size) >= 0);

    buff[buff_size - 1] = '\0';
    printf("Read `%s'\n", buff);

    pi_assert(
            strcmp(pattern, buff) == 0);

    pi_l2_free(buff, buff_size);

    puts("Test read partition 1 done");
}

void open_partition(pi_device_t *part, struct pi_partition_conf *conf, uint8_t id, pi_device_t *flash)
{
    printf("Open partition %u\n", id);

    conf->id = id;
    conf->flash = flash;
    part->config = conf;
    pi_assert(
            pi_partition_open(part) >= 0);
}

void open_flash(pi_device_t *flash, struct pi_hyperflash_conf *flash_conf)
{
    pi_hyperflash_conf_init(flash_conf);
    pi_open_from_conf(flash, flash_conf);
    pi_assert(
            pi_flash_open(flash) >= 0);
}

void test_partition(void)
{
    pi_device_t flash;
    struct pi_hyperflash_conf flash_conf;
    pi_device_t part0, part1;
    struct pi_partition_conf part0_conf, part1_conf;
    size_t part0_size, part1_size;
    uint32_t part0_offset, part1_offset;

    printf("Entering main controller\n");

    puts("Open flash");
    open_flash(&flash, &flash_conf);

    puts("\nTest open and check partitions");
    // Open partition 0 : Firmware
    open_partition(&part0, &part0_conf, 0, &flash);
    part0_offset = pi_partition_get_flash_offset(&part0);
    part0_size = pi_partition_get_size(&part0);
    printf("Partition 0: size 0x%zx, flash offset 0x%lx\n", part0_size, part0_offset);
    pi_assert(part0_offset == 4);
    pi_assert(part0_size != 0);
    pi_assert(part0_size != UINT32_MAX);

    // Open partition 1 Filesystem
    open_partition(&part1, &part1_conf, 1, &flash);
    part1_offset = pi_partition_get_flash_offset(&part1);
    part1_size = pi_partition_get_size(&part1);
    printf("Partition 1: size 0x%zx, flash offset 0x%lx\n", part1_size, part1_offset);
    pi_assert(part1_size != 0);
    pi_assert(part1_size != UINT32_MAX);

    pi_assert(part0_offset + part0_size == part1_offset);
    puts("Test open and check partitions done");

    test_read_partition(&part1);
    test_write_partition(&part1);
    test_erase_partition(&part1);
    test_bad_addr(&part1);

    pi_flash_close(&flash);

    printf("\nTest partition success!\n");
    pmsis_exit(0);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Partition test ***\n\n");
    return pmsis_kickoff((void *) test_partition);
}
