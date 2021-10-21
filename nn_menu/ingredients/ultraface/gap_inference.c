#include "pmsis.h"
#include "bsp/fs.h"
#include "bsp/fs/readfs.h"
#include "bsp/fs/hostfs.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/ram/hyperram.h"
#include "gaplib/ImgIO.h"
#include "setup.h"
#include <stdio.h>
#include "nms.h"

#define MAX_OUT_BBOX 30
#define SCORE_THRESHOLD 0.7
#define q_box_scale 0.0099901
#define q_score_scale 0.00003052

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

struct bbox{
   int x1;
   int y1;
   int x2;
   int y2;
   int score;
};

struct bbox detected_objects[MAX_OUT_BBOX];
unsigned char image[INPUT_SIZE];

char raw_text[512];

int atoi(char* str)
{
    int res = 0;
    int sign = 1;
    int i = 0;

    if (str[0] == '-')
    {
        sign = -1;
        i++;
    }

    for (; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    return sign * res;
}

int load_and_parse_boxes(struct pi_device* host_fs, char* boxes_file, struct bbox* boxes)
{
    int box_counter = 0;
    pi_fs_file_t* host_file = pi_fs_open(host_fs, boxes_file, PI_FS_FLAGS_READ);
    if (host_file == NULL)
    {
        PRINTF("Failed to open file, %s\n", boxes_file);
        pmsis_exit(-7);
    }
    int read = pi_fs_read(host_file, raw_text, 512);

    char* startl = raw_text;
    char* endl;
    while(endl = strchr(startl, '\n'))
    {
        int l, t, r, b, s;
        *endl = '\0';

        char* separator = strchr(startl, ';');
        *separator = '\0';
        l = atoi(startl);
        startl = separator+1;

        separator = strchr(startl, ';');
        *separator = '\0';
        t = atoi(startl);
        startl = separator+1;

        separator = strchr(startl, ';');
        *separator = '\0';
        r = atoi(startl);
        startl = separator+1;

        separator = strchr(startl, ';');
        *separator = '\0';
        b = atoi(startl);
        startl = separator+1;

        s = atoi(startl);

        boxes[box_counter].x1 = l;
        boxes[box_counter].y1 = t;
        boxes[box_counter].x2 = r;
        boxes[box_counter].y2 = b;
        boxes[box_counter].score = s;
        printf("atoi results\n");
        printf("%d; %d; %d; %d; %d;\n", l, t, r, b, s);

        box_counter++;
        startl = endl+1;
    }

    pi_fs_close(host_file);

    return 0;
}


void DrawRectangle(unsigned char *Img, int W, int H, int x, int y, int w, int h, unsigned char* Value)
{
    int x0, x1, y0, y1;

    y0 = MAX(MIN(y, H - 1), 0);
    y1 = MAX(MIN(y + h - 1, H - 1), 0);

    x0 = x;
    if (x0 >= 0 && x0 < W) {
        for (int i = y0; i <= y1; i++)
        {
            Img[3 * (i * W + x0)] = Value[0];
            Img[3 * (i * W + x0) + 1] = Value[1];
            Img[3 * (i * W + x0) + 2] = Value[2];
        }
    }

    x1 = x + w - 1;
    if (x1 >= 0 && x1 < W) {
        for (int i = y0; i <= y1; i++)
        {
            Img[3 * (i * W + x1)] = Value[0];
            Img[3 * (i * W + x1) + 1] = Value[1];
            Img[3 * (i * W + x1) + 2] = Value[2];
        }
    }

    x0 = MAX(MIN(x, W - 1), 0);
    x1 = MAX(MIN(x + w - 1, W - 1), 0);

    y0 = y;
    if (y0 >= 0 && y0 < H) {
        for (int i = x0; i <= x1; i++)
        {
            Img[3 * (y0 * W + i)] = Value[0];
            Img[3 * (y0 * W + i) + 1] = Value[1];
            Img[3 * (y0 * W + i) + 2] = Value[2];
        }
    }

    y1 = y + h - 1;
    if (y1 >= 0 && y1 < H) {
        for (int i = x0; i <= x1; i++)
        {
            Img[3 * (y1 * W + i)] = Value[0];
            Img[3 * (y1 * W + i) + 1] = Value[1];
            Img[3 * (y1 * W + i) + 2] = Value[2];
        }
    }
}


void body(void* parameters)
{
    (void) parameters;

    PRINTF("Init cluster...\n");
    //Setting FC to 250MHz
    //pi_freq_set(PI_FREQ_DOMAIN_FC, 50000000);

    //Setting Cluster to 175MHz
    // NOTE: Current Gap8 generation does not have clock divider for hyperbus
    // and using FC clocks over 150Mhz is dangerous
    //pi_freq_set(PI_FREQ_DOMAIN_CL, 150000000);

    struct pi_hostfs_conf host_fs_conf;
    pi_hostfs_conf_init(&host_fs_conf);
    struct pi_device host_fs;
    pi_open_from_conf(&host_fs, &host_fs_conf);

    if (pi_fs_mount(&host_fs))
    {
        PRINTF("pi_fs_mount failed\n");
        pmsis_exit(-4);
    }

    char boxes_file[128];
    sprintf(boxes_file, "../../../quant_boxes.csv");
    pi_fs_file_t* host_file = pi_fs_open(&host_fs, boxes_file, PI_FS_FLAGS_READ);
    int status = load_and_parse_boxes(&host_fs, boxes_file, detected_objects);

    int input_size = IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(unsigned char);
    char* filename = "../../../image.ppm";

    if (ReadImageFromFile(filename,
        IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANNELS,
        image, INPUT_SIZE*sizeof(unsigned char), IMGIO_OUTPUT_CHAR, 0))
    {
        PRINTF("Image read failed\n");
        pmsis_exit(-2);
    }
    printf("Finished reading image\n");

    int boxes_total = 0;
    printf("Boxes\n");
    float out_boxes[MAX_OUT_BBOX * 5];
    int j=0;
    for(int i = 0; i < MAX_OUT_BBOX; i++)
    {
        float score = detected_objects[i].score * q_score_scale;
        float x1 = detected_objects[i].x1 * q_box_scale;
        float y1 = detected_objects[i].y1 * q_box_scale;
        float x2 = detected_objects[i].x2 * q_box_scale;
        float y2 = detected_objects[i].y2 * q_box_scale;

        x1 *= IMAGE_WIDTH;
        y1 *= IMAGE_HEIGHT;
        x2 *= IMAGE_WIDTH;
        y2 *= IMAGE_HEIGHT;

        if(score < SCORE_THRESHOLD)
            continue;

        printf("%d; %f; %f; %f; %f; %f;\n", i, x1, y1, x2, y2, score);
        out_boxes[j] = x1;
        out_boxes[j + 1] = y1;
        out_boxes[j + 2] = x2;
        out_boxes[j + 3] = y2;
        out_boxes[j + 4] = score;
        j += 5;

        boxes_total++;
    }

    // output each array element's value
    for (int i = 0; i < MAX_OUT_BBOX * 5; i++ ) {
        printf("boxes[%d] = %f\n", i, out_boxes[i]);
    }
    int keep_out[MAX_OUT_BBOX];
    int order[MAX_OUT_BBOX];
    float areas[MAX_OUT_BBOX];
    for (int i=0; i < MAX_OUT_BBOX; i++){
        order[i] = i;
    }

    printf("Boxes total: %d\n", boxes_total);

    int boxes_after_nms = cpu_nms(keep_out, out_boxes, order, 0.5);
    printf("Boxes after NMS: %d\n", boxes_after_nms);

    unsigned char Value[3] = {0, 255, 0};
    for (int i=0; i < boxes_after_nms; i++)
    {
        int index = keep_out[i];
        printf("index: %d\n", index);
        printf("box: \n");
        float x1 = out_boxes[5 * index];
        float y1 = out_boxes[5 * index + 1];
        float x2 = out_boxes[5 * index + 2];
        float y2 = out_boxes[5 * index + 3];
        float score = out_boxes[5 * index + 4];
        printf("x1: %f\n", x1); 
        printf("y1: %f\n", y1);
        printf("x2: %f\n", x2);
        printf("y2: %f\n", y2);
        printf("score: %f \n", score);
        DrawRectangle(image, IMAGE_WIDTH, IMAGE_HEIGHT, (int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1), Value);
    }


    WriteImageToFile("../../../boxes.pgm", IMAGE_WIDTH, IMAGE_HEIGHT, 3, image, RGB888_IO);

    pi_fs_close(host_file);

    pi_fs_unmount(&host_fs);
    PRINTF("FS unmount done!\n");

    pmsis_exit(0);
}

int main()
{
    return pmsis_kickoff(body);
}
