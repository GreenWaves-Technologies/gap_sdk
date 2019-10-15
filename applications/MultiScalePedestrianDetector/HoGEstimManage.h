#ifndef __HOG_ESTIMMANAGE_H__
#define __HOG_ESTIMMANAGE_H__
typedef struct {
	unsigned short int X;
	unsigned short int Y;
	unsigned short int W;
	unsigned short int H;
	unsigned short int Merit;
} BoundingBox;

int AllocateBBList(
        unsigned int N
	);

void ResetBBList();
void DumpBBList();

void DrawBBList(
	unsigned char* image,
	unsigned int W,
	unsigned int H);

int PushBB(
	unsigned int X,
	unsigned int Y,
	unsigned int W,
	unsigned int H,
	unsigned int Merit,
	unsigned int NonMax);

int PushBBAt(
        unsigned int XFeat,
        unsigned int YFeat,
        unsigned int Merit,
	unsigned int NonMax);

void SetBBAspectRatio(
        unsigned int DimRef,
        unsigned int Dim);

void RemoveNonMaxBB();

#endif
