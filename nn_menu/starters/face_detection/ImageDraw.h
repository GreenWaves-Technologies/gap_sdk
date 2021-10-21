#include "DataTypes.h"

void GrayImageToRGB(Pixu *In, Pixu *Out, int W, int H);

void DrawLine(Pixu *Img, int H, int W, int x0, int y0, int x1, int y1, Pixu Value);
void DrawLineRGB(Pixu *Img, int H, int W, int x0, int y0, int x1, int y1, Pixu R, Pixu G, Pixu B);
void DrawRectangle(Pixu *Img, int H, int W, int x, int y, int w, int h, Pixu Value);
void DrawRectangleRGB(Pixu *Img, int H, int W, int x, int y, int w, int h, Pixu R, Pixu G, Pixu B);
