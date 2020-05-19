#include <stdio.h>

int main(int argc, char* argv[]) {

  FILE* infile; // fb.raw
  FILE* outfile; // fb.ppm
  unsigned char red, green, blue; // 8-bits each
  unsigned short pixel; // 16-bits per pixel
  unsigned int maxval; // max color val
  unsigned short width, height;
  size_t i;

  infile = fopen("./output.raw", "r");
  outfile = fopen("./output.ppm", "wb");
  width = 320;
  height = 240;
  maxval = 255;

  // P3 - PPM "plain" header
  fprintf(outfile, "P3\n#created with rgb2ppm\n%d %d\n%d\n", width, height, maxval);

  for (i = 0; i < width * height; i += 1) {
      fread(&pixel, sizeof(unsigned short), 1, infile);

      red = (unsigned short)((pixel & 0xF800) >> 11);  // 5
      green = (unsigned short)((pixel & 0x07E0) >> 5); // 6
      blue = (unsigned short)(pixel & 0x001F);         // 5

      // Increase intensity
      red = red << 3;
      green = green << 2;
      blue = blue << 3;

    // P6 binary
    //fwrite(&(red | green | blue), 1, sizeof(unsigned short), outfile);

    // P3 "plain"
    fprintf(outfile, "%d %d %d\n", red, green, blue);
  }
}
