#ifndef __AES_LIB_H__
#define __AES_LIB_H__

#define MAXBC         (4)

// #define MAXBC         (4)
#define MAXKC         (4)
#define MAXROUNDS     (10)
#define BC            (4)
#define KC            (4)
#define ROUNDS        (10)

typedef struct {
  unsigned char mul2[256];
  unsigned char mul3[256];
  unsigned char S[256];
  unsigned char RC[30];
  unsigned char RoundKey[MAXROUNDS+1][4][4];
} aes_data_t;


extern void AesBuildLUT(aes_data_t *data);
extern void AesCtrCipher(
    	aes_data_t *data,
		unsigned char *In,
		unsigned char *Out,
		unsigned char Key[4][MAXBC],
		unsigned char IV[2][MAXBC],
		int Len,
		int Reset);



#endif
