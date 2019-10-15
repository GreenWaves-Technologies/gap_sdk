#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gap_common.h"
#include "ImgIO.h"

char buffer[8192];

int main() {

    unsigned int width = 322, height = 242;

    printf("FC mbed_os Bridge test\n");

    BRIDGE_Init();

    printf("Connecting to bridge\n");

    BRIDGE_Connect(0, NULL);

    printf("Connection done\n");

    unsigned char *InBuffer = (unsigned char *) malloc( width*height*sizeof(unsigned char));

    ReadImageFromFile("../../../Pedestrian.ppm",&width,&height,InBuffer,width*height*sizeof(unsigned char));
    WriteImageToFile("../../../Pedestrian_OUT.ppm",width,height,InBuffer);

    BRIDGE_Disconnect(NULL);
    printf("Test success\n");

    return 0;
}
