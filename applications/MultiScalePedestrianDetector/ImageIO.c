/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#include "DataTypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __pulp__
#include <dirent.h>
#endif
#ifdef WITH_PNG
#include "PngIO.h"
#endif



void writePPM(const char *filename, Pixu *img, int W, int H, int isRGB)

{
        FILE *fp;
        //open file for output
        fp = fopen(filename, "w");
        if (!fp) {
                fprintf(stderr, "Unable to open file '%s'\n", filename); exit(1);
        }
        //write the header file
        //image format
	if (isRGB) {
        	fprintf(fp, "P6\n");
        	//image size
        	fprintf(fp, "%d %d\n", W, H);
        	// rgb component depth, here 255 grey levels
        	fprintf(fp, "%d\n",255);
        	// pixel data
        	fwrite(img, 3, W*H, fp);
	} else {
        	fprintf(fp, "P5\n");
        	//image size
        	fprintf(fp, "%d %d\n", W, H);
        	// rgb component depth, here 255 grey levels
        	fprintf(fp, "%d\n",255);
        	// pixel data
        	fwrite(img, 1, W*H, fp);
	}
        fclose(fp);
}

void writePaddedPPM(const char *filename, Pixu *img, int W, int H, int PadW, int PadH, int isRGB)

{
        FILE *fp;
	int i;
        fp = fopen(filename, "w");
        if (!fp) {
                fprintf(stderr, "Unable to open file '%s'\n", filename); exit(1);
        }
        //write the header file
        //image format
	if (isRGB) {
        	fprintf(fp, "P6\n");
        	//image size
        	fprintf(fp, "%d %d\n", W, H);
        	// rgb component depth, here 255 grey levels
        	fprintf(fp, "%d\n",255);
        	// pixel data
		for (i=0; i<H; i++) fwrite(&img[((W+PadW*2)*(i+PadH)+PadW)*3], 3, W, fp);
	} else {
        	fprintf(fp, "P5\n");
        	//image size
        	fprintf(fp, "%d %d\n", W, H);
        	// rgb component depth, here 255 grey levels
        	fprintf(fp, "%d\n",255);
        	// pixel data
		for (i=0; i<H; i++) fwrite(&img[((W+PadW*2)*(i+PadH)+PadW)], 1, W, fp);
	}
        fclose(fp);
}

void writePPM_HR(const char *filename, Pixuw *img, int W, int H, int isRGB)

{
        FILE *fp;
        //open file for output
        fp = fopen(filename, "w");
        if (!fp) {
                fprintf(stderr, "Unable to open file '%s'\n", filename); exit(1);
        }
        //write the header file
        //image format
	if (isRGB) {
        	fprintf(fp, "P6\n");
        	//image size
        	fprintf(fp, "%d %d\n", W, H);
        	// rgb component depth, here 65535 grey levels
        	fprintf(fp, "%d\n",65535);
        	// pixel data
        	fwrite(img, 3*2, W*H, fp);
	} else {
        	fprintf(fp, "P5\n");
        	//image size
        	fprintf(fp, "%d %d\n", W, H);
        	// rgb component depth, here 65535 grey levels
        	fprintf(fp, "%d\n",65535);
        	// pixel data
        	fwrite(img, 2, W*H, fp);
	}
        fclose(fp);
}

Pixu *Read_Ppm_Pgm_Image(char *file_name, int *W, int *H, int RetRGB, unsigned char *InBuffer, unsigned int BuffSize)


{
        FILE *file_in;
        char buff[16];
        char c;
        unsigned char *buffer=NULL;
	int RetItem=RetRGB?3:1;
        int size_item;
        int Color;
        int Is_RGB;
	int Cnt;

#define  RGB_TO_GRAY(x,n)  (((x) + (1 << ((n)-1))) >> (n))
#define  SCALE  14
#define  cR  (int)(0.299*(1 << SCALE) + 0.5)
#define  cG  (int)(0.587*(1 << SCALE) + 0.5)
#define  cB  ((1 << SCALE) - cR - cG)

        file_in = fopen(file_name, "r");
        if (file_in == NULL) {
                printf("Error: Can't open file %s\n", file_name); return NULL;
        }
        if (!fgets(buff, sizeof(buff), file_in)) {
              printf("Error: Reading file %s\n", file_name); fclose(file_in); return NULL;
        }
        if (buff[0] != 'P' || (buff[1] != '6' && buff[1] != '5')) {
                fprintf(stderr, "Invalid image format (must be 'P6' or 'P5')\n");
                fclose(file_in); return NULL;
        } else if (buff[1] == '5') size_item = 1; // Gray
        else size_item = 3; // RGB
        Is_RGB = (size_item == 3);
        c = getc(file_in);
        while (c == '#') {
                while (getc(file_in) != '\n') ;
                c = getc(file_in);
        }
        ungetc(c, file_in);
        if ((Cnt = fscanf(file_in, "%d %d ", W, H)) != 2) {
                printf("Error: wrong ppm/pgm file format (W, H), Got %d Values, expect 2, File: %s\n", Cnt, file_name); fclose(file_in); return NULL;
        }
        c = getc(file_in);
        while (c == '#') {
                while (getc(file_in) != '\n') ;
                c = getc(file_in);
        }
        ungetc(c, file_in);
        if (fscanf(file_in, "%d", &Color) != 1) {
                printf("Error: wrong ppm file format (Color, expect 255)\n"); fclose(file_in); return NULL;
        }
        while (fgetc(file_in) != '\n') ;
	if (InBuffer && (BuffSize >= (((*W)*(*H)*RetItem)))) buffer = InBuffer;
	else buffer = (unsigned char *) malloc(sizeof(unsigned char)*(((*W)*(*H)*RetItem)));
        if (Is_RGB == RetRGB) {
                if (fread(buffer, size_item*(*W),  *H, file_in) !=  *H) {
                        printf("Error: Failed to read image section of %s\n", file_name);
                        free(buffer);
                        fclose(file_in); return NULL;
                }
        } else {
                unsigned char pixel[3];
                int i=0;
                while (fread(pixel, 1, size_item,  file_in) ==  size_item) {
			if (RetItem==1) buffer[i++] = RGB_TO_GRAY(pixel[2]*cB + pixel[1]*cG + pixel[0]*cR, SCALE);
			else {
				buffer[i++] = pixel[0]; buffer[i++] = pixel[0]; buffer[i++] = pixel[0];
			}
                }
                if (i != (*W)*(*H)*RetItem) {
                        printf("Error: Failed to read image section of %s\n", file_name);
                        free(buffer);
                        fclose(file_in); return NULL;
                }
	}
        fclose(file_in);
        return buffer;
#undef  RGB_TO_GRAY
#undef  SCALE
#undef  cR
#undef  cG
#undef  cB
}

void PadImage(Pixu *Img, int W, int H, int IsRGB, int PadW, int PadH, int PadRule, Pixu *FromImg)

{
	int LineSize  = (W+(PadW<<1));
	int LineCount = H+(PadH<<1);
	int Channels= IsRGB?3:1;
	int i, j, k;

        if (FromImg) {
                for (i=0; i<H; i++)
                        for (j=0; j<W; j++)
				for (k=0; k<Channels; k++)
					Img[((i+PadH)*LineSize+j+PadW)*Channels+k] = FromImg[(i*W+j)*Channels+k];
        }

        if (PadRule == 0) {
                /* Pad with 0 */
                for (i=0; i<PadH; i++)
                        for (j=1; j<(LineSize-1); j++)
				for (k=0; k<Channels; k++) {
                                	Img[(i*LineSize + j)*Channels+k] = 0;
					Img[((LineCount - i - 1)*LineSize + j)*Channels+k] = 0;
				}
                for (i=0; i<PadW; i++) 
                        for (j=0; j<LineCount; j++) 
				for (k=0; k<Channels; k++) {
                                	Img[(j*LineSize + i)*Channels+k] = 0;
					Img[(j*LineSize + LineSize - i - 1)*Channels+k] = 0;
				}
        } else {
                /* Pad with closest existing value */
                for (i=0; i<PadH; i++) 
                        for (j=1; j<(LineSize-1); j++)
				for (k=0; k<Channels; k++) {
                                	Img[(i*LineSize + j)*Channels+k] = Img[(PadH*LineSize+j)*Channels+k];
                                	Img[((LineCount-i-1)*LineSize+j)*Channels+k] = Img[((LineCount-PadH-1)*LineSize+j)*Channels+k];
				}
                for (i=0; i<PadW; i++)
                        for (j=0; j<LineCount; j++)
				for (k=0; k<Channels; k++) {
                                	Img[(j*LineSize+i)*Channels+k] = Img[(j*LineSize+PadW)*Channels+k];
                                	Img[(j*LineSize+LineSize-i-1)*Channels+k] = Img[(j*LineSize+LineSize-PadW-1)*Channels+k];
				}
        }
}

Pixu *Read_Padded_Ppm_Pgm_Image(char *file_name, int *W, int *H, int RetRGB, int PadW, int PadH, int PadRule)

{
        FILE *file_in;
        char buff[16];
        char c;
        unsigned char *buffer=NULL;
        int size_item;
	int RetItem=RetRGB?3:1;
        int Color;
        int Is_RGB;
	int Cnt;

#define  RGB_TO_GRAY(x,n)  (((x) + (1 << ((n)-1))) >> (n))
#define  SCALE  14
#define  cR  (int)(0.299*(1 << SCALE) + 0.5)
#define  cG  (int)(0.587*(1 << SCALE) + 0.5)
#define  cB  ((1 << SCALE) - cR - cG)

        file_in = fopen(file_name, "r");
        if (file_in == NULL) {
                printf("Error: Can't open file %s\n", file_name); return NULL;
        }
        if (!fgets(buff, sizeof(buff), file_in)) {
              printf("Error: Reading file %s\n", file_name); fclose(file_in); return NULL;
        }
        if (buff[0] != 'P' || (buff[1] != '6' && buff[1] != '5')) {
                fprintf(stderr, "Invalid image format (must be 'P6' or 'P5')\n");
                fclose(file_in); return NULL;
        } else if (buff[1] == '5') size_item = 1; // Gray
        else size_item = 3; // RGB
        Is_RGB = (size_item == 3);
        c = getc(file_in);
        while (c == '#') {
                while (getc(file_in) != '\n') ;
                c = getc(file_in);
        }
        ungetc(c, file_in);
        if ((Cnt = fscanf(file_in, "%d %d ", W, H)) != 2) {
                printf("Error: wrong ppm/pgm file format (W, H), Got %d Values, expect 2, File: %s\n", Cnt, file_name); fclose(file_in); return NULL;
        }
        c = getc(file_in);
        while (c == '#') {
                while (getc(file_in) != '\n') ;
                c = getc(file_in);
        }
        ungetc(c, file_in);
        if (fscanf(file_in, "%d", &Color) != 1) {
                printf("Error: wrong ppm file format (Color, expect 255)\n"); fclose(file_in); return NULL;
        }
        while (fgetc(file_in) != '\n') ;

       	buffer = (unsigned char *) malloc(sizeof(unsigned char)*(((*W+(PadW<<1))*(*H + (PadH<<1)))*RetItem + 4));
        if (Is_RGB==RetRGB) {
		int h = 0;
		int LineSize  = *W+(PadW<<1);
		unsigned char *Line;
		int i, j;

 		Line = buffer + (PadW+LineSize*PadH)*size_item;
		while (fread(Line, size_item,  *W, file_in) ==  *W) {
			h++;
			Line += LineSize*size_item;
		}
		if (h!=*H) {
                        printf("Error: Failed to read image section of %s\n", file_name);
                        free(buffer);
                        fclose(file_in); return NULL;
                }
        } else {
                unsigned char pixel[3];
                int i=0, j, k;
		int LineSize  = *W+(PadW<<1);
		int Off = PadW+LineSize*PadH;

		for (j=0; j<*H; j++) {
			k = 0;
                	while (k<*W && fread(pixel, 1, size_item, file_in) ==  size_item) {
				if (RetItem==1) buffer[Off + i++] = RGB_TO_GRAY(pixel[2]*cB + pixel[1]*cG + pixel[0]*cR, SCALE);
				else {
					buffer[Off + i++] = pixel[0]; buffer[Off + i++] = pixel[0]; buffer[Off + i++] = pixel[0];
				}
				k++;
                	}
			if (k!=*W) break;
			Off += (PadW<<1)*RetItem;
		}
                if (i != (*W)*(*H)*RetItem) {
                        printf("Error: Failed to read image section of %s\n", file_name);
                        free(buffer);
                        fclose(file_in); return NULL;
                }
        }
	PadImage(buffer, *W, *H, RetRGB, PadW, PadH, PadRule, NULL);
        fclose(file_in);
        return buffer;
#undef  RGB_TO_GRAY
#undef  SCALE
#undef  cR
#undef  cG
#undef  cB
}

Pixu *RGBto4Channels(Pixu *In, int W, int H)

{
#define  RGB_TO_GRAY(x,n)  (((x) + (1 << ((n)-1))) >> (n))
#define  SCALE  14
#define  cR  (int)(0.299*(1 << SCALE) + 0.5)
#define  cG  (int)(0.587*(1 << SCALE) + 0.5)
#define  cB  ((1 << SCALE) - cR - cG)

	int ImgSize = W*H;
	Pixu *Out = (Pixu *) malloc(sizeof(Pixu)*ImgSize*4 + 4);
	int i;

	for (i=0; i<ImgSize; i++) {
		Out[0*ImgSize + i] = RGB_TO_GRAY(In[3*i+2]*cB + In[3*i+1]*cG + In[3*i+0]*cR, SCALE);
		Out[1*ImgSize + i] = In[3*i];
		Out[2*ImgSize + i] = In[3*i+1];
		Out[3*ImgSize + i] = In[3*i+2];

	}
	return Out;

#undef  RGB_TO_GRAY
#undef  SCALE
#undef  cR
#undef  cG
#undef  cB
}

Pixu *RGBtoYUV(Pixu *In, int W, int H)

{
#define SCALE 14
#ifdef __EMUL__
#define DOTUSP(a, b) ((a)[0]*(b)[0] + (a)[1]*(b)[1])
#define SDOTUSP(a, b, c) (c + (a)[0]*(b)[0] + (a)[1]*(b)[1])
#else
#define DOTUSP(a, b) __builtin_pulp_dotusp2(a, b)
#define SDOTUSP(a, b, c) __builtin_pulp_sdotusp2(a, b, c)
#endif

        int ImgSize = W*H;
        Pixu *Out = (Pixu *) malloc(sizeof(Pixu)*ImgSize*3 + 4);
        int i;


        Pixsw CCoeff[] = {
                (Pixsw) (( 65.481/255.0)*(1<<SCALE) + 0.5), (Pixsw) ((128.553/255.0)*(1<<SCALE) + 0.5), (Pixsw) (( 25.966/255.0)*(1<<SCALE) + 0.5), 0, /* RGB to Y coeffs */
                (Pixsw) ((-37.945/255.0)*(1<<SCALE) + 0.5), (Pixsw) ((-74.203/255.0)*(1<<SCALE) + 0.5), (Pixsw) ((112.000/255.0)*(1<<SCALE) + 0.5), 0, /* RGB to U coeffs */
                (Pixsw) ((112.000/255.0)*(1<<SCALE) + 0.5), (Pixsw) ((-93.786/255.0)*(1<<SCALE) + 0.5), (Pixsw) ((-18.214/255.0)*(1<<SCALE) + 0.5), 0  /* RGB to V coeffs */
        };
        int OffsetAndRound[] = {
                (( 16<<SCALE) + (1<<SCALE-1)),
                ((128<<SCALE) + (1<<SCALE-1))
        };
        PixsVw *Coeff = (PixsVw *) CCoeff;

        for (i=0; i<ImgSize; i++) {
                PixuV V = *((PixuV *) &In[3*i]);
                PixuVw W1 = {V[0], V[1]}; /* {R, G} */
                PixuVw W2 = {V[2], 0};    /* {B, 0} */

                Out[0*ImgSize + i] = (OffsetAndRound[0] + SDOTUSP(W2, Coeff[1], DOTUSP(W1, Coeff[0])))>>SCALE;
                Out[1*ImgSize + i] = (OffsetAndRound[1] + SDOTUSP(W2, Coeff[3], DOTUSP(W1, Coeff[2])))>>SCALE;
                Out[2*ImgSize + i] = (OffsetAndRound[1] + SDOTUSP(W2, Coeff[5], DOTUSP(W1, Coeff[4])))>>SCALE;
        }

        return Out;

#undef SCALE
#undef DOTUSP
#undef SDOTUSP

}


#ifdef WITH_PNG
Pixu *Read_Png_Image(const char *file_name, int *W, int *H, int AsRGB)

{
	int IsRgb = 0;
	Pixu *Img = NULL;
	unsigned error;

	if (AsRGB) error = lodepng_decode_file(&Img, W, H, file_name, LCT_RGB, 8);
	else error = lodepng_decode_file(&Img, W, H, file_name, LCT_GREY, 8);

	if (error) {
		printf("Failed to read PNG %s, decoder error %u: %s\n", file_name, error, lodepng_error_text(error));
		Img = NULL;
	}
	return Img;

}
#endif

#ifndef __pulp

static int CmpStringp(const void *p1, const void *p2)

{
        return strcmp(* (char * const *) p1, * (char * const *) p2);
}

char **GetAllFileNamesInDirectory(char *DirectoryName, char *Extension, int *N)

{
	DIR *Dir;
	struct dirent *Ent;
	int n=0;
	char **List;
	

	if ((Dir = opendir (DirectoryName)) != NULL) {
  		while ((Ent = readdir (Dir)) != NULL) {
			if (Extension == NULL || (strstr(Ent->d_name, Extension)!=NULL)) n++;
		}
		List = (char **) malloc(n*sizeof(char *));
		if (List == NULL) {
			printf("Error: GetAllFileNamesInDirectory, failed to allocate\n"); return NULL;
		}
		rewinddir(Dir);
		n = 0;
  		while ((Ent = readdir (Dir)) != NULL) {
			if (Extension == NULL || (strstr(Ent->d_name, Extension)!=NULL)) {
				List[n] = (char *) malloc(sizeof(char)*(strlen(Ent->d_name)+strlen(DirectoryName)+2));
				if (List[n] == NULL) {
					printf("Error: GetAllFileNamesInDirectory, failed to allocate\n"); return NULL;
				}
				sprintf(List[n], "%s/%s", DirectoryName, Ent->d_name);
				n++;
			}
		}
		*N = n;
  		closedir (Dir);
		
	
	} else printf("Error: Failed to open directory %s\n", DirectoryName);
	*N = n;
	qsort(List, n, sizeof(char *), CmpStringp);
	return List;
}
#endif
