#include <math.h>
#include <stdio.h>

int cpu_nms(int* keep_out, float* boxes, int* order, float nms_overlap_thresh)
{
    // boxes has to be sorted
    //THArgCheck(THLongTensor_isContiguous(keep_out), 0, "keep_out must be contiguous");
    //THArgCheck(THLongTensor_isContiguous(boxes), 2, "boxes must be contiguous");
    //THArgCheck(THLongTensor_isContiguous(order), 3, "order must be contiguous");
    //THArgCheck(THLongTensor_isContiguous(areas), 4, "areas must be contiguous");
    // Number of ROIs
    int boxes_num = 8;
    int boxes_dim = 5;

    int suppressed[boxes_num];
    float areas[boxes_num];
    float x1, y1, x2, y2, area;
    for (int i=0; i<boxes_num; i++)
    {
        suppressed[i] = 0;
        x1 = boxes[i * boxes_dim];
        y1 = boxes[i * boxes_dim + 1];
        x2 = boxes[i * boxes_dim + 2];
        y2 = boxes[i * boxes_dim + 3];
        area = (x2 - x1) * (y2 - y1);
        areas[i] = area; 
    }
    //THByteTensor* suppressed = THByteTensor_newWithSize1d(boxes_num);
    //THByteTensor_fill(suppressed, 0);
    //unsigned char * suppressed =  THByteTensor_data(suppressed);

    // nominal indices
    int i, j;
    // sorted indices
    int _i, _j;
    // temp variables for box i's (the box currently under consideration)
    // variables for computing overlap with box j (lower scoring box)
    float xx1, yy1, xx2, yy2;
    float w, h;
    float inter, ovr;

    int num_out = 0;
    for (_i=0; _i < boxes_num; ++_i) {
        i = order[_i];
        if (suppressed[i] == 1) {
            continue;
        }
        keep_out[num_out++] = i;
        x1 = boxes[i * boxes_dim];
        y1 = boxes[i * boxes_dim + 1];
        x2 = boxes[i * boxes_dim + 2];
        y2 = boxes[i * boxes_dim + 3];
        area = areas[i];
        for (_j = _i + 1; _j < boxes_num; ++_j) {
            j = order[_j];
            if (suppressed[j] == 1) {
                continue;
            }
            xx1 = fmaxf(x1, boxes[j * boxes_dim]);
            yy1 = fmaxf(y1, boxes[j * boxes_dim + 1]);
            xx2 = fminf(x2, boxes[j * boxes_dim + 2]);
            yy2 = fminf(y2, boxes[j * boxes_dim + 3]);
            w = fmaxf(0.0, xx2 - xx1 + 1);
            h = fmaxf(0.0, yy2 - yy1 + 1);
            inter = w * h;
            ovr = inter / (area + areas[j] - inter);
            if (ovr >= nms_overlap_thresh) {
                suppressed[j] = 1;
            }
        }
    }

    for (int i=0; i < num_out; i++)
    {
        printf("keep_out[%d]: %d\n", i, keep_out[i]);
    }
    return num_out;
}