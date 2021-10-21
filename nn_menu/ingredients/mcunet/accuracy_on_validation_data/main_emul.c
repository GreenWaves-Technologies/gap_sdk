#include "gaplib/ImgIO.h"
#include "Gap.h"
#include "setup.h"
#include "MCUNetKernels.h"

PI_L2 signed char network_input[INPUT_SIZE];
PI_L2 signed char network_output[NETWORK_CLASSES];
char *gap_result = NULL;
char *filename = NULL;

void body()
{
    switch_fs_t host_fs;
    __FS_INIT(host_fs);

    PRINTF("Writing output to %s\n", gap_result);
    switch_file_t* host_file = __OPEN_WRITE(&host_fs, gap_result);
    if (host_file == NULL)
    {
        PRINTF("Failed to open file, %s\n", gap_result);
        exit(-5);
    }

    int input_size = IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(char);
    //char* dir_name = "quant_data";

    if (ReadImageFromFile(filename,
        IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_CHANNELS,
        network_input, INPUT_SIZE*sizeof(char), IMGIO_OUTPUT_CHAR, 0))
    {
        PRINTF("Image read failed\n");
        exit(-2);
    }
    printf("Finished reading image\n");

    int status = MCUNetCNN_Construct();
    PRINTF("Detector initialized with status %d\n", status);
    if (status)
    {
        exit(-4);
    }
    PRINTF("Network init done\n");

    MCUNetCNN(network_input, network_output);    

    MCUNetCNN_Destruct();

    short int outclass = 0;
    signed char max_score = -127;
    for(int i=0; i < NETWORK_CLASSES; i++){
    //PRINTF("%d\n", network_output[i]);
       if (network_output[i] > max_score){
          outclass = i;
          max_score = network_output[i];
       }
    }
    PRINTF("Predicted class:\t%d\n", outclass + 1);
    PRINTF("With confidence:\t%d\n", max_score);

    char buf[2];
    for(int i=0; i < NETWORK_CLASSES; i++){
       sprintf(buf, "%d;", network_output[i]);
       //printf("%s\n", buf);
       __WRITE(host_file, buf, strlen(buf));
    }
    __CLOSE(host_file);

    __FS_DEINIT(&host_fs);

    exit(0);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        PRINTF("Usage: exe [image_file] [gap_result_file]\n");
        exit(-1);
    }
    filename = argv[1];
    gap_result = argv[2];
    body();
}
