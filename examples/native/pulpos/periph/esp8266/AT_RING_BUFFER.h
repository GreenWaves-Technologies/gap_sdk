#include <stdio.h>
#include <rt/rt_api.h>
#include "Gap8.h"
#include <stdint.h>

int AT_COMMAND_Init(int baudrate);

void AT_COMMAND_Write(char *command, int length);

int AT_COMMAND_Read(char *r_buf, int length);
