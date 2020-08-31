#ifndef _READ_INPUT_H
#define _READ_INPUT_H

int OpenSamplesFile(char *FileName);
int GetNextSample(short int *Samples, int SampleCount);
void CloseSamplesFiles();

#endif
