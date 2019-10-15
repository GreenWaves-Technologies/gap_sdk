#ifndef __AES_LIB_H__
#define __AES_LIB_H__

#define MAXBC         (4)

extern void AesBuildLUT();
extern void AesCtrCipher(
		unsigned char *In,
		unsigned char *Out,
		unsigned char Key[4][MAXBC],
		unsigned char IV[2][MAXBC],
		int Len,
		int Reset);
#endif
