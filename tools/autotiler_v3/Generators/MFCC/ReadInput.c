#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sndfile.h>


static const char *
sfe_endian_name (int format)
{
	switch (format & SF_FORMAT_ENDMASK)
	{	case SF_ENDIAN_FILE : return "file" ;
		case SF_ENDIAN_LITTLE : return "little" ;
		case SF_ENDIAN_BIG : return "big" ;
		case SF_ENDIAN_CPU : return "cpu" ;
		default : break ;
		} ;

	return "unknown" ;
} /* sfe_endian_name */

static const char *
sfe_container_name (int format)
{
	switch (format & SF_FORMAT_TYPEMASK)
	{	case SF_FORMAT_WAV : return "WAV" ;
		case SF_FORMAT_AIFF : return "AIFF" ;
		case SF_FORMAT_AU : return "AU" ;
		case SF_FORMAT_RAW : return "RAW" ;
		case SF_FORMAT_PAF : return "PAF" ;
		case SF_FORMAT_SVX : return "SVX" ;
		case SF_FORMAT_NIST : return "NIST" ;
		case SF_FORMAT_VOC : return "VOC" ;
		case SF_FORMAT_IRCAM : return "IRCAM" ;
		case SF_FORMAT_W64 : return "W64" ;
		case SF_FORMAT_MAT4 : return "MAT4" ;
		case SF_FORMAT_MAT5 : return "MAT5" ;
		case SF_FORMAT_PVF : return "PVF" ;
		case SF_FORMAT_XI : return "XI" ;
		case SF_FORMAT_HTK : return "HTK" ;
		case SF_FORMAT_SDS : return "SDS" ;
		case SF_FORMAT_AVR : return "AVR" ;
		case SF_FORMAT_WAVEX : return "WAVEX" ;
		case SF_FORMAT_SD2 : return "SD2" ;
		case SF_FORMAT_FLAC : return "FLAC" ;
		case SF_FORMAT_CAF : return "CAF" ;
		case SF_FORMAT_WVE : return "WVE" ;
		case SF_FORMAT_OGG : return "OGG" ;
		case SF_FORMAT_MPC2K : return "MPC2K" ;
		case SF_FORMAT_RF64 : return "RF64" ;
		default : break ;
		} ;

	return "unknown" ;
} /* sfe_container_name */

static const char *
sfe_codec_name (int format)
{
	switch (format & SF_FORMAT_SUBMASK)
	{	case SF_FORMAT_PCM_S8 : return "signed 8 bit PCM" ;
		case SF_FORMAT_PCM_16 : return "16 bit PCM" ;
		case SF_FORMAT_PCM_24 : return "24 bit PCM" ;
		case SF_FORMAT_PCM_32 : return "32 bit PCM" ;
		case SF_FORMAT_PCM_U8 : return "unsigned 8 bit PCM" ;
		case SF_FORMAT_FLOAT : return "32 bit float" ;
		case SF_FORMAT_DOUBLE : return "64 bit double" ;
		case SF_FORMAT_ULAW : return "u-law" ;
		case SF_FORMAT_ALAW : return "a-law" ;
		case SF_FORMAT_IMA_ADPCM : return "IMA ADPCM" ;
		case SF_FORMAT_MS_ADPCM : return "MS ADPCM" ;
		case SF_FORMAT_GSM610 : return "gsm610" ;
		case SF_FORMAT_VOX_ADPCM : return "Vox ADPCM" ;
		case SF_FORMAT_G721_32 : return "g721 32kbps" ;
		case SF_FORMAT_G723_24 : return "g723 24kbps" ;
		case SF_FORMAT_G723_40 : return "g723 40kbps" ;
		case SF_FORMAT_DWVW_12 : return "12 bit DWVW" ;
		case SF_FORMAT_DWVW_16 : return "16 bit DWVW" ;
		case SF_FORMAT_DWVW_24 : return "14 bit DWVW" ;
		case SF_FORMAT_DWVW_N : return "DWVW" ;
		case SF_FORMAT_DPCM_8 : return "8 bit DPCM" ;
		case SF_FORMAT_DPCM_16 : return "16 bit DPCM" ;
		case SF_FORMAT_VORBIS : return "Vorbis" ;
		case SF_FORMAT_ALAC_16 : return "16 bit ALAC" ;
		case SF_FORMAT_ALAC_20 : return "20 bit ALAC" ;
		case SF_FORMAT_ALAC_24 : return "24 bit ALAC" ;
		case SF_FORMAT_ALAC_32 : return "32 bit ALAC" ;
		default : break ;
		} ;
	return "unknown" ;
} /* sfe_codec_name */

#define	BLOCK_SIZE 512
#define MAX_CHANNELS 5

static SNDFILE		*infile = NULL ;
static int Channels;

int GetNextSample(short int *Samples, int SampleCount)

{
	int SampleRead;
	int Value;
	int i, j;

	if (infile == NULL) {
		printf("Error: Non audio sample file open\n"); return 0;
	}
	SampleRead = sf_readf_short (infile, Samples, SampleCount);
	if (SampleRead == 0) return 0;

	if (SampleRead < SampleCount) {
		for (i=0; i<SampleCount; i++) Samples[i] = 0;
	}
	
	if (Channels > 1) {
		for (i=0; i<SampleCount; i++) {
			Value = 0;
			for (j=0; j<Channels; j++) Value = Samples[i*Channels + j];
			Samples[i] = Value/Channels;
		}
	}
	return 1;
}

int OpenSamplesFile(char *FileName)

{
	SF_INFO	sfinfo ;
	double	max;

	if ((infile = sf_open (FileName, SFM_READ, &sfinfo)) == NULL) {
		printf ("Not able to open input file %s.\n", FileName) ;
		puts (sf_strerror (NULL)) ;
		return 0;
	}

	sf_command (infile, SFC_CALC_SIGNAL_MAX, &max, sizeof (max)) ;
	printf ("Container: %s, Codec: %s, Endianess: %s, Channels: %d, Sample rate: %d, Frames: %ld, Max Value: %f\n",
		 sfe_container_name(sfinfo.format), sfe_codec_name(sfinfo.format), sfe_endian_name(sfinfo.format),
		 sfinfo.channels, sfinfo.samplerate, sfinfo.frames, max) ;
	Channels = sfinfo.channels;
	if (Channels > MAX_CHANNELS) {
		printf("Max number of channels exceeded %d, Max: %d\n", Channels, MAX_CHANNELS);
		return 0;
	}
	return 1;

}

void CloseSamplesFiles()

{
	sf_close (infile) ;
	infile = NULL;
}
