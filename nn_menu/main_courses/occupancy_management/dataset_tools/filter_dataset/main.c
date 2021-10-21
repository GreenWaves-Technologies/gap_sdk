/*
 * Copyright 2020 GreenWaves Technologies, SAS
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "limits.h"
#include "shutterless.h"

#define WIDTH 80
#define HEIGHT 80
#define GAMMA 1

void writePGM(char * filename,uint8_t*image,int width, int height){

	FILE* pgmimg; 
	pgmimg = fopen(filename, "wb"); 

    // Writing Magic Number to the File 
	fprintf(pgmimg, "P2\n");  

    // Writing Width and Height 
	fprintf(pgmimg, "%d %d\n", width, height);  

    // Writing the maximum gray value 
	fprintf(pgmimg, "255\n");  
	int count = 0; 
	for (int i = 0; i < height; i++) { 
		for (int j = 0; j < width; j++) { 
			uint8_t temp = image[i*width+j]; 

            // Writing the gray values in the 2D array to the file 
			fprintf(pgmimg, "%d ", temp); 
		} 
		fprintf(pgmimg, "\n"); 
	} 
	fclose(pgmimg); 
}


int read_raw_image(char* fileName, uint16_t *img_buffer,int width, int height){
	FILE *f_bin;
	int ret;
	f_bin = fopen(fileName,"rb");
	ret=fread(img_buffer,2,width*height,f_bin);
	fclose(f_bin);
	return ret;
}

int write_raw_image(char* fileName, uint16_t *img_buffer,int width, int height){
	FILE *f_bin;
	int ret;

	f_bin = fopen(fileName,"wb");
	ret=fwrite(img_buffer,2,width*height,f_bin);
	fclose(f_bin);

	return ret;
}


void process_img(char * in_file_dir,char* in_file_name,char* offset_fn, char* output_folder_raw,char* output_folder_pgm){

	char in_fn [100];
	sprintf(in_fn,"%s/%s",in_file_dir,in_file_name);
	char out_raw_fn [100];
	sprintf(out_raw_fn,"%s/%s",output_folder_raw,in_file_name);
	char out_pgm_fn [100];
	sprintf(out_pgm_fn,"%s/%s.pgm",output_folder_pgm,in_file_name);


	uint16_t* input       = (uint16_t*) malloc(WIDTH*HEIGHT*sizeof(uint16_t)); 
	uint16_t* offset      = (uint16_t*) malloc(WIDTH*HEIGHT*sizeof(uint16_t));
	uint16_t* output_raw  = (uint16_t*) malloc(WIDTH*HEIGHT*sizeof(uint16_t));
	uint8_t*  output_pgm  = (uint8_t*) malloc(WIDTH*HEIGHT*sizeof(uint8_t));


	read_raw_image(in_fn, input, WIDTH, HEIGHT);
	read_raw_image(offset_fn, offset, WIDTH, HEIGHT); //This could be done once instead that every time


	Shutterless(input, offset, output_raw, output_pgm, GAMMA);

	write_raw_image(out_raw_fn, output_raw,WIDTH, HEIGHT);
	writePGM(out_pgm_fn,output_pgm,WIDTH, HEIGHT);


	free(input);
	free(offset);
	free(output_raw);
	free(output_pgm);

	return;
}

int main(int argc, char** argv)
{
	struct dirent *dp;
	DIR *dfd;
	char output_folder_raw[] = "filtered_raw"; 
	char output_folder_pgm[] = "filtered_pgm"; 


	char *dir ;
	dir = argv[2] ;

	if ( argc != 3 )
	{
		printf("Usage: %s offset_file input_dirname\n",argv[0]);
		return 0;
	}

	if ((dfd = opendir(dir)) == NULL)
	{
		fprintf(stderr, "Can't open %s\n", dir);
		return 0;
	}

	char filename_qfd[300] ;
	char new_name_qfd[300] ;

	while ((dp = readdir(dfd)) != NULL)
	{
		struct stat stbuf ;
		sprintf( filename_qfd , "%s/%s",dir,dp->d_name) ;
		if( stat(filename_qfd,&stbuf ) == -1 )
		{
			printf("Unable to stat file: %s\n",filename_qfd) ;
			continue ;
		}
		else
		{
			if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0 )
				continue;

			process_img(dir,dp->d_name,argv[1],output_folder_raw,output_folder_pgm);

			//printf("file name: %s\n",dp->d_name);
			//printf("%s/%s\n",dir,dp->d_name);
   		}
	}
}
