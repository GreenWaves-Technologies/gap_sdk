#include <stdio.h>
#include "AesLib.h"

// #define MAXBC         (4)
#define MAXKC         (4)
#define MAXROUNDS     (10)
#define BC            (4)
#define KC            (4)
#define ROUNDS        (10)

typedef unsigned char  v4u __attribute__((vector_size (4)));

#define packu4(x, y, z, t)   ((v4u) {(unsigned char) (x), (unsigned char) (y), (unsigned char) (z), (unsigned char) (t)})

unsigned char mul2[256];
unsigned char mul3[256];
unsigned char S[256];
unsigned char RC[30];
unsigned char RoundKey[MAXROUNDS+1][4][4];

static unsigned char GF8_Mul(unsigned char a, unsigned char b)

{
	unsigned char p = 0; /* the product of the multiplication */
	while (b) {
		/* if b is odd, then add the corresponding a to p (final product = sum of all a's corresponding to odd b's) */
		if (b & 1) p ^= a; /* since we're in GF(2^m), addition is an XOR */
		/* GF modulo: if a >= 128, then it will overflow when shifted left, so reduce */
		if (a & 0x80) a = (a << 1) ^ 0x11b; /* XOR with the primitive polynomial x^8 + x^4 + x^3 + x + 1 */
            	else a <<= 1; /* equivalent to a*2 */
            	b >>= 1; /* equivalent to b // 2 */
	}
	return p;
}

static unsigned char GF8_rcon(unsigned char in)

{
	unsigned char c=1;

	if (in == 0)  return 0; 
	while(in != 1) {
		c = GF8_Mul(c,2); in--;
	}
	return c;
}

static void GenerateAES_LUTs(unsigned char *Sbox, unsigned char *GF8_mul2, unsigned char *GF8_mul3, unsigned char *Rcon)

{
	unsigned char p = 1, q = 1;
	int i, j;

	for (i=0; i<16; i++) {
		for (j=0; j<16; j++) {
			GF8_mul2[16*i+j] = GF8_Mul(16*i+j, 2);
			GF8_mul3[16*i+j] = GF8_Mul(16*i+j, 3);
		}
	}

#define ROTL8(x,shift) ((unsigned char) ((x) << (shift)) | ((x) >> (8 - (shift))))
	/* loop invariant: p * q == 1 in the Galois field */
	do {
		/* multiply p by 2 */
		p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);
		/* divide q by 2 */
		q ^= q << 1; q ^= q << 2; q ^= q << 4; q ^= q & 0x80 ? 0x09 : 0;
		/* compute the affine transformation */
		unsigned char xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);
		Sbox[p] = xformed ^ 0x63;
	} while (p != 1);
	/* 0 is a special case since it has no inverse */
	Sbox[0] = 0x63;
#undef ROTL8
	for (i=0; i<30; i++) Rcon[i] = GF8_rcon(i);
}


static inline void KeyExpansion(unsigned char Key[4][MAXBC])

{
  	int j, t, rconpointer = 1;

  	((v4u *) RoundKey[0])[0] = ((v4u *) Key)[0];
  	((v4u *) RoundKey[0])[1] = ((v4u *) Key)[1];
  	((v4u *) RoundKey[0])[2] = ((v4u *) Key)[2];
  	((v4u *) RoundKey[0])[3] = ((v4u *) Key)[3];

  	for(t = 1; t < (ROUNDS+1); ++t) {
    		v4u V;
    		Key[0][0] ^= S[Key[1][KC-1]];
    		Key[1][0] ^= S[Key[2][KC-1]];
    		Key[2][0] ^= S[Key[3][KC-1]];
    		Key[3][0] ^= S[Key[0][KC-1]];
    		Key[0][0] ^= RC[rconpointer++];

    		Key[0][1] ^= Key[0][0];
    		Key[1][1] ^= Key[1][0];
    		Key[2][1] ^= Key[2][0];
    		Key[3][1] ^= Key[3][0];
    		Key[0][2] ^= Key[0][1];
    		Key[1][2] ^= Key[1][1];
    		Key[2][2] ^= Key[2][1];
    		Key[3][2] ^= Key[3][1];
    		Key[0][3] ^= Key[0][2];
    		Key[1][3] ^= Key[1][2];
    		Key[2][3] ^= Key[2][2];
    		Key[3][3] ^= Key[3][2];

    		((v4u *) RoundKey[t])[0] = ((v4u *) Key)[0];
    		((v4u *) RoundKey[t])[1] = ((v4u *) Key)[1];
    		((v4u *) RoundKey[t])[2] = ((v4u *) Key)[2];
    		((v4u *) RoundKey[t])[3] = ((v4u *) Key)[3];
  	}
}

static inline void Encrypt (unsigned char State[4][MAXBC])

{
	unsigned char tmp2[4][4];
  	int r;

  	// Add round key
  	((v4u *) State)[0] = ((v4u *) State)[0] ^ ((v4u *) RoundKey[0])[0];
  	((v4u *) State)[1] = ((v4u *) State)[1] ^ ((v4u *) RoundKey[0])[1];
  	((v4u *) State)[2] = ((v4u *) State)[2] ^ ((v4u *) RoundKey[0])[2];
  	((v4u *) State)[3] = ((v4u *) State)[3] ^ ((v4u *) RoundKey[0])[3];

  	for(r = 1; r < ROUNDS; r++) {
    		v4u V, V0, V1, V2, V3;

    		V = ((v4u *) State)[0]; ((v4u *) State)[0] = packu4(S[V[0]], S[V[1]], S[V[2]], S[V[3]]); 
    		V = ((v4u *) State)[1]; ((v4u *) State)[1] = packu4(S[V[0]], S[V[1]], S[V[2]], S[V[3]]); 
    		V = ((v4u *) State)[2]; ((v4u *) State)[2] = packu4(S[V[0]], S[V[1]], S[V[2]], S[V[3]]); 
    		V = ((v4u *) State)[3]; ((v4u *) State)[3] = packu4(S[V[0]], S[V[1]], S[V[2]], S[V[3]]); 

    		// Shift Rows
    		((v4u *) State)[1] = __builtin_shuffle(((v4u *) State)[1], (v4u){1,2,3,0});
    		((v4u *) State)[2] = __builtin_shuffle(((v4u *) State)[2], (v4u){2,3,0,1});
    		((v4u *) State)[3] = __builtin_shuffle(((v4u *) State)[3], (v4u){3,0,1,2});
		
    		// MixColumns
    		V0 = ((v4u *) State)[0]; V1 = ((v4u *) State)[1]; V2 = ((v4u *) State)[2]; V3 = ((v4u *) State)[3];
    		((v4u *) tmp2)[0] = packu4(mul2[V0[0]], mul2[V0[1]], mul2[V0[2]], mul2[V0[3]]) ^
				    packu4(mul3[V1[0]], mul3[V1[1]], mul3[V1[2]], mul3[V1[3]]) ^
				    V2 ^ V3;
    		((v4u *) tmp2)[1] = packu4(mul2[V1[0]], mul2[V1[1]], mul2[V1[2]], mul2[V1[3]]) ^
				    packu4(mul3[V2[0]], mul3[V2[1]], mul3[V2[2]], mul3[V2[3]]) ^
			            V3 ^ V0;
    		((v4u *) tmp2)[2] = packu4(mul2[V2[0]], mul2[V2[1]], mul2[V2[2]], mul2[V2[3]]) ^
				    packu4(mul3[V3[0]], mul3[V3[1]], mul3[V3[2]], mul3[V3[3]]) ^
				    V0 ^ V1;
    		((v4u *) tmp2)[3] = packu4(mul2[V3[0]], mul2[V3[1]], mul2[V3[2]], mul2[V3[3]]) ^
				    packu4(mul3[V0[0]], mul3[V0[1]], mul3[V0[2]], mul3[V0[3]]) ^
				    V1 ^ V2;
    		((v4u *) State)[0] = ((v4u *) tmp2)[0];
    		((v4u *) State)[1] = ((v4u *) tmp2)[1];
    		((v4u *) State)[2] = ((v4u *) tmp2)[2];
    		((v4u *) State)[3] = ((v4u *) tmp2)[3];

    		// Add round key
    		((v4u *) State)[0] = ((v4u *) State)[0] ^ ((v4u *) RoundKey[r])[0];
    		((v4u *) State)[1] = ((v4u *) State)[1] ^ ((v4u *) RoundKey[r])[1];
    		((v4u *) State)[2] = ((v4u *) State)[2] ^ ((v4u *) RoundKey[r])[2];
    		((v4u *) State)[3] = ((v4u *) State)[3] ^ ((v4u *) RoundKey[r])[3];
  	}
  	// Last round without mixcolumn
  	{
		v4u V;
  		V = ((v4u *) State)[0]; ((v4u *) State)[0] = packu4(S[V[0]], S[V[1]], S[V[2]], S[V[3]]); 
  		V = ((v4u *) State)[1]; ((v4u *) State)[1] = packu4(S[V[0]], S[V[1]], S[V[2]], S[V[3]]); 
  		V = ((v4u *) State)[2]; ((v4u *) State)[2] = packu4(S[V[0]], S[V[1]], S[V[2]], S[V[3]]); 
  		V = ((v4u *) State)[3]; ((v4u *) State)[3] = packu4(S[V[0]], S[V[1]], S[V[2]], S[V[3]]); 
  	}
  	// Shift Rows
  	((v4u *) State)[1] = __builtin_shuffle(((v4u *) State)[1], (v4u){1,2,3,0});
  	((v4u *) State)[2] = __builtin_shuffle(((v4u *) State)[2], (v4u){2,3,0,1});
  	((v4u *) State)[3] = __builtin_shuffle(((v4u *) State)[3], (v4u){3,0,1,2});

  	// Add round key
  	((v4u *) State)[0] = ((v4u *) State)[0] ^ ((v4u *) RoundKey[ROUNDS])[0];
  	((v4u *) State)[1] = ((v4u *) State)[1] ^ ((v4u *) RoundKey[ROUNDS])[1];
  	((v4u *) State)[2] = ((v4u *) State)[2] ^ ((v4u *) RoundKey[ROUNDS])[2];
  	((v4u *) State)[3] = ((v4u *) State)[3] ^ ((v4u *) RoundKey[ROUNDS])[3];
}

void AesCtrCipher(
		unsigned char *In,
		unsigned char *Out,
		unsigned char Key[4][MAXBC],
		unsigned char IV[2][MAXBC],
		int Len,
		int Reset)

{
	int i, j;
	unsigned char Buff[16];
	unsigned char State[4][4];
	int Counter = 0;
	v4u *Vo = (v4u *) Out;
	v4u *Vi = (v4u *) In;
	v4u *VBuff = (v4u *) Buff;

	if (Reset) KeyExpansion(Key);

	((unsigned int *) State)[0] = ((unsigned int *) IV)[0];
	((unsigned int *) State)[1] = ((unsigned int *) IV)[1];
	((unsigned int *) State)[2] = 0;

	for (i=0; i<Len/16; i++) {
		((unsigned int *) State)[3] = Counter++;
		Encrypt(State);
		for (j=0; j<4; j++) *Vo++ = *Vi++ ^ ((v4u *) State)[j];
	}
	((unsigned int *) State)[3] = Counter++;
	Encrypt(State);
	VBuff[0] = (v4u) {0, 0, 0, 0}; VBuff[1] = (v4u) {0, 0, 0, 0}; VBuff[2] = (v4u) {0, 0, 0, 0}; VBuff[3] = (v4u) {0, 0, 0, 0};
	for (i=0; i<(Len%16); i++) Buff[i] = In[(Len&(~0x0F)) + i];
	for (i=0; i<4; i++) VBuff[i] = VBuff[i] ^ ((v4u *) State)[i];
	for (j=0, i=Len & (~0x0F); i<Len; i++, j++) Out[i] = Buff[j];

//printf("Constants: %d, Work: %d\n", sizeof(mul2)+sizeof(mul3)+sizeof(S)+sizeof(RC), sizeof(RoundKey));
}

void AesBuildLUT()

{
	GenerateAES_LUTs(S, mul2, mul3, RC);
}
