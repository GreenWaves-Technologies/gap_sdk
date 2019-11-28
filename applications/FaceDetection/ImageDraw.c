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


void GrayImageToRGB(Pixu *In, Pixu *Out, int W, int H)

{
        int i;

        for (i=0; i<(W*H); i++) {
                Out[3*i]=Out[3*i+1]=Out[3*i+2]=In[i];
        }
}

void DrawLineRGB(Pixu *Img, int H, int W, int x0, int y0, int x1, int y1, Pixu R, Pixu G, Pixu B)

{
#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
        int i, j, w, h;
        int rem, x, y;
/*
        x0 = Max(x0, 0); x0 = Min(x0, W-1);
        y0 = Max(y0, 0); y0 = Min(y0, H-1);
        x1 = Max(x1, 0); x1 = Min(x1, W-1);
        y1 = Max(y1, 0); y1 = Min(y1, H-1);
*/
        if (x1 < x0) {
                int t;
                t = x1; x1=x0; x0=t;
                t = y1; y1=y0; y0=t;
        }
        if (y1 < y0) {
                h = (y0 - y1)+1; w = (x1 - x0)+1;
                if (w < h) {
                        rem = 0; y=y0;
                        for (i = x0; i<=x1; i++) {
                                for (j = 0; j<((h+rem)/w); j++) {
                                        int P = i + (y--)*W;
                                        if (P>=0&&P<(H*W)) {
                                                Img[3*P] = R; Img[3*P+1] = G; Img[3*P+2] = B;
                                        }
                                }
                                rem = (h+rem)%w;
                        }
                } else {
                        rem = 0; x = x0;
                        for (j = y0; j>=y1;j--) {
                                for (i = 0; i<((w+rem)/h); i++) {
                                        int P = x++ + j*W;
                                        if (P>=0&&P<(H*W)) {
                                                Img[3*P] = R; Img[3*P+1] = G; Img[3*P+2] = B;
                                        }
                                }
                                rem = (w+rem)%h;
                        }

                }
        } else {
                h = (y1 - y0)+1; w = (x1 - x0)+1;
                if (w < h) {
                        rem = 0; y=y0;
                        for (i = x0; i<=x1; i++) {
                                for (j = 0; j<((h+rem)/w); j++) {
                                        int P = i + (y++)*W;
                                        if (P>=0&&P<(H*W)) {
                                                Img[3*P] = R; Img[3*P+1] = G; Img[3*P+2] = B;
                                        }
                                }
                                rem = (h+rem)%w;
                        }
                } else {
                        rem = 0; x = x0;
                        for (j = y0; j<=y1;j++) {
                                for (i = 0; i<((w+rem)/h); i++) {
                                        int P = x++ + j*W;
                                        if (P>=0&&P<(H*W)) {
                                                Img[3*P] = R; Img[3*P+1] = G; Img[3*P+2] = B;
                                        }
                                }
                                rem = (w+rem)%h;
                        }
                }
        }
#undef Max
#undef Min
}

void DrawLine(Pixu *Img, int H, int W, int x0, int y0, int x1, int y1, Pixu Value)

{
#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
        int i, j, w, h;
        int rem, x, y;

        x0 = Max(x0, 0); x0 = Min(x0, W-1);
        y0 = Max(y0, 0); y0 = Min(y0, H-1);
        x1 = Max(x1, 0); x1 = Min(x1, W-1);
        y1 = Max(y1, 0); y1 = Min(y1, H-1);

        if (x1 < x0) {
                int t;
                t = x1; x1=x0; x0=t;
                t = y1; y1=y0; y0=t;
        }
        if (y1 < y0) {
                h = (y0 - y1)+1; w = (x1 - x0)+1;
                if (w < h) {
                        rem = 0; y=y0;
                        for (i = x0; i<=x1; i++) {
                                for (j = 0; j<((h+rem)/w); j++) {
                                        int P = i + (y--)*W;
                                        if (P>=0&&P<(H*W)) Img[P] = Value;
                                }
                                rem = (h+rem)%w;
                        }
                } else {
                        rem = 0; x = x0;
                        for (j = y0; j>=y1;j--) {
                                for (i = 0; i<((w+rem)/h); i++) {
                                        int P = x++ + j*W;
                                        if (P>=0&&P<(H*W)) Img[P] = Value;
                                }
                                rem = (w+rem)%h;
                        }

                }
        } else {
                h = (y1 - y0)+1; w = (x1 - x0)+1;
                if (w < h) {
                        rem = 0; y=y0;
                        for (i = x0; i<=x1; i++) {
                                for (j = 0; j<((h+rem)/w); j++) {
                                        int P = i + (y++)*W;
                                        if (P>=0&&P<(H*W)) Img[P] = Value;
                                }
                                rem = (h+rem)%w;
                        }
                } else {
                        rem = 0; x = x0;
                        for (j = y0; j<=y1;j++) {
                                for (i = 0; i<((w+rem)/h); i++) {
                                        int P = x++ + j*W;
                                        if (P>=0&&P<(H*W)) Img[P] = Value;
                                }
                                rem = (w+rem)%h;
                        }
                }
        }
#undef Max
#undef Min
}

void DrawRectangle(Pixu *Img, int H, int W, int x, int y, int w, int h, Pixu Value)

{
#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
        int i;
        int x1 = Min(x+w-1, W-1);
        int y1 = Min(y+h-1, H-1);

        x = Max(Min(x, W-1), 0);
        y = Max(Min(y, H-1), 0);
        for (i=x; i<x1; i++) {Img[y*W + i] = Value; Img[y1*W + i] = Value;}
        for (i=y; i<=y1; i++) {Img[i*W + x] = Value; Img[i*W + x1] = Value;}
#undef Max
#undef Min
}

void DrawRectangleRGB(Pixu *Img, int H, int W, int x, int y, int w, int h, Pixu R, Pixu G, Pixu B)

{
#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))
        int i;
        int x1 = Min(x+w-1, W-1);
        int y1 = Min(y+h-1, H-1);

        x = Max(Min(x, W-1), 0);
        y = Max(Min(y, H-1), 0);
        for (i=x; i<x1; i++) {
                Img[3*y*W + 3*i] = R; Img[3*y*W + 3*i+1] = G; Img[3*y*W + 3*i+2] = B;
                Img[3*y1*W + 3*i] = R; Img[3*y1*W + 3*i+1] = G; Img[3*y1*W + 3*i+2] = B;
        }
        for (i=y; i<=y1; i++) {
                Img[3*i*W + 3*x] = R; Img[3*i*W + 3*x+1] = G; Img[3*i*W + 3*x+2] = B;
                Img[3*i*W + 3*x1] = R; Img[3*i*W + 3*x1+1] = G; Img[3*i*W + 3*x1+2] = B;
        }
#undef Max
#undef Min
}
