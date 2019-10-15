#include <stdio.h>
#include <string.h>
#include "AesLib.h"
#include <stdlib.h>

static void DumpMessage(char *Mess, unsigned char *Text, int Len, int Asc)

{
        int i;

        printf("%s:\n\t", Mess);
        for (i=0; i<Len; i++) {
                if (Asc) printf("%c", Text[i]); else printf("%2x", (unsigned char) Text[i]);
                if (((i+1)%64) == 0) printf("\n\t");
        }
        printf("\n");
        printf("===========================================00\n");
}

char *HexImage(unsigned char *In, int Len)

{
	static char Str[256];
	char Hex[8];
	int i;

	Str[0] = 0;
	for (i=0; i<Len; i++) {
		sprintf(Hex, "%2x ", In[i]);
		if (Hex[0] == ' ') Hex[0] = '0';
		strcat(Str, Hex);
	}
	return Str;
}

int main(int argc, char *argv[])

{
	if (argc != 3) return -1;
	if (strlen(argv[1]) != 32) return -1;
	if (strlen(argv[2]) != 16) return -1;

	unsigned char IV[2][MAXBC];
	unsigned char Key[4][MAXBC];

	((unsigned int *) Key)[0] = strtol(&argv[1][24], NULL, 16); argv[1][24] = 0;
	((unsigned int *) Key)[1] = strtol(&argv[1][16], NULL, 16); argv[1][16] = 0;
	((unsigned int *) Key)[2] = strtol(&argv[1][8], NULL, 16); argv[1][8] = 0;
	((unsigned int *) Key)[3] = strtol(&argv[1][0], NULL, 16);

	((unsigned int *) IV)[0] = strtol(&argv[2][8], NULL, 16); argv[2][8] = 0;
	((unsigned int *) IV)[1] = strtol(&argv[2][0], NULL, 16);

	int bufferSize = 16*1024*1024;
	unsigned char *buffer = malloc(bufferSize);
	bufferSize = fread(buffer, 1, bufferSize, stdin);

	//printf("Initialization Vector: %s\n", HexImage((unsigned char *) IV, 8));
	//printf("AES128 Key           : %s\n", HexImage((unsigned char *) Key, 16));

	AesBuildLUT();




	AesCtrCipher(buffer, buffer, Key, IV, bufferSize, 1);

	fwrite(buffer, 1, bufferSize, stdout);


    //DumpMessage("Ciphered", buffer, bufferSize, 0);
	//AesCtrCipher(buffer, buffer, Key, IV, bufferSize, 0);
    //DumpMessage("Un Ciphered", buffer, bufferSize, 1);


	return 0;
}

