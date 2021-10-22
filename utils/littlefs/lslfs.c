/*
 * Copyright (C) 2020 GreenWaves Technologies
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
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 1/10/2020.
 */



#include <stdio.h>
#include "stdbool.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#include "lfs/lfs.h"

static struct lfs_config cfg;
static lfs_t lfs;
static uint8_t *data;
static size_t data_size = 0;

static int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	if(block * c->block_size + off + size > data_size)
		return LFS_ERR_IO;
	memcpy(buffer, data + (block * c->block_size) + off, size);
	return LFS_ERR_OK;
}

static int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	if(block * c->block_size + off + size > data_size)
		return LFS_ERR_IO;
	memcpy(data + (block * c->block_size) + off, buffer, size);
	return LFS_ERR_OK;
}

static int lfs_erase(const struct lfs_config *c, lfs_block_t block)
{
	if(block * c->block_size + c->block_size > data_size)
		return LFS_ERR_IO;
	memset(data + (block * c->block_size), 0xFF, c->block_size);
	return LFS_ERR_OK;
}

static int lfs_sync(const struct lfs_config *c)
{
	return LFS_ERR_OK;
}

static int lfs_config(struct lfs_config *lfs_config, void *context)
{
	lfs_config->context = context;
	
	lfs_config->read = lfs_read;
	lfs_config->prog = lfs_prog;
	lfs_config->erase = lfs_erase;
	lfs_config->sync = lfs_sync;
	
	lfs_config->read_size = 4;
	lfs_config->prog_size = 4;
	lfs_config->block_size = 256 << 10;
	lfs_config->block_count = 32 * 1; // Must be a multiple of 32
	lfs_config->block_cycles = 100;
	
	lfs_config->cache_size = 1024;
	lfs_config->lookahead_size = lfs_config->block_count;
	
	return 0;
}

int list_dir(lfs_t *lfs, const char *dir_name, const uint8_t level, const bool recursive)
{
	int rc;
	uint8_t nbr_of_files = 0;
	lfs_dir_t dir;
	struct lfs_info info;
	
	rc = lfs_dir_open(lfs, &dir, dir_name);
	if(rc < 0)
	{
		printf("Little FS open dir %s error %d\r\n", dir_name, rc);
		exit(-1);
	}
	
	if(level == 0)
		printf("%s\n", dir_name);
	
	while ((rc = lfs_dir_read(lfs, &dir, &info)) > 0)
	{
		for (int i = 0; i <= level; i++)
			printf("  ");
		printf("%s", info.name);
		nbr_of_files++;
		if(info.type == LFS_TYPE_DIR
		   && strcmp(".", info.name)
		   && strcmp("..", info.name))
		{
			printf("/\r\n");
			if(recursive)
				nbr_of_files += list_dir(lfs, info.name, level + 1, recursive);
		} else
			printf("\r\n");
	}
	if(rc < 0)
	{
		printf("Little FS read dir error %d\r\n", rc);
		exit(-1);
	}
	
	rc = lfs_dir_close(lfs, &dir);
	if(rc < 0)
	{
		printf("Little FS close dir error %d\r\n", rc);
		exit(-1);
	}
	return nbr_of_files;
}

int main(int argc, char **argv)
{
	char *src = NULL;
	int err;
	
	if(argc != 2)
	{
		fprintf(stderr, "LittleFS ls usage:\n"
		                "lslfs <lfs-image>\n");
		exit(-1);
	}
	
	src = argv[1];
	
	// Mount the file system
	lfs_config(&cfg, NULL);
	
	FILE *fimg = fopen(src, "rb");
	if(!fimg)
	{
		fprintf(stderr, "can't open image file: errno=%d (%s)\r\n", errno, strerror(errno));
		return -1;
	}
	
	/*
	 * Fetch the size of LFS image.
	 *
	 * Firsty, go to the end of the file.
	 * */
	if(fseek(fimg, 0L, SEEK_END) != 0)
	{
		fputs("Error reading input file", stderr);
		exit(-1);
	}
	
	/* Get the size of the file. */
	data_size = ftell(fimg);
	if(data_size == -1)
	{
		fputs("Error reading input file", stderr);
		exit(-1);
	}
	printf("Image size %ldBytes\n", data_size);
	
	/* Allocate our buffer to that size. */
	data = malloc(data_size + 1);
	if(data == NULL)
	{
		fputs("Allooc data buffer error.\n", stderr);
		exit(-1);
	}
	
	/* Go back to the start of the file. */
	fseek(fimg, 0L, SEEK_SET);
	
	/* Read the entire file into memory. */
	size_t newLen = fread(data, sizeof(char), data_size, fimg);
	if(ferror(fimg) != 0)
	{
		fputs("Error reading input file", stderr);
		exit(-1);
	} else
	{
		data[newLen++] = '\0'; /* Just to be safe. */
	}
	fclose(fimg);
	
	err = lfs_mount(&lfs, &cfg);
	if(err == LFS_ERR_IO)
	{
		fputs("Error: Input file is not a valid LFS image.\n", stderr);
		exit(-1);
	}
	if(err < 0)
	{
		fprintf(stderr,
		        "mount error: error=%d\r\n", err);
		return -1;
	}
	
	puts("LittleFS image mounted");
	
	list_dir(&lfs, "/", 0, true);
	
	lfs_unmount(&lfs);
	free(data);
}
