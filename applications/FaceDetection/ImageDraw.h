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

void GrayImageToRGB(Pixu *In, Pixu *Out, int W, int H);

void DrawLine(Pixu *Img, int H, int W, int x0, int y0, int x1, int y1, Pixu Value);
void DrawLineRGB(Pixu *Img, int H, int W, int x0, int y0, int x1, int y1, Pixu R, Pixu G, Pixu B);
void DrawRectangle(Pixu *Img, int H, int W, int x, int y, int w, int h, Pixu Value);
void DrawRectangleRGB(Pixu *Img, int H, int W, int x, int y, int w, int h, Pixu R, Pixu G, Pixu B);
