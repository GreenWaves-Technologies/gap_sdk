# Key Word Spotting Example

The processing step for audio has three steps :

 - Collecting audio raw data from audio with format of PCM or PDM
 - Do MFCC processing which will generate pre-precessing data
 - Do CNN calculation with CLUSTER or with HWCE and do classification.

## How to use

 - If you want to use audio, which will do all the three steps, in Makefile just uncomment line 19 and line 45. It will capture your voice each time (pronouce once when capturing audio) and does a recognization. And for step 3, you can use CLUSTER or HWCE to do the CNN, if you want to use CLUSTER, comment line 15, 16, uncomment line 12. If you want to use HWCE, vice versa.

 - If you do not want to use the captured audio (BY DEFAULT), just run the test, it will use a test audio file to do the last two steps. And for step 3, you can use CLUSTER or HWCE to do the CNN, if you want to use CLUSTER, comment line 15, 16, uncomment line 12. If you want to use HWCE, vice versa. Comment line 19 and line 45

 - If you just want to do step 3, comment line 18, then the test will use pre-processing data after step 2 to do the CNN. And for step 3, you can use CLUSTER or HWCE to do the CNN, if you want to use CLUSTER, comment line 15, 16, uncomment line 12. If you want to use HWCE, vice versa. Comment line 19 and line 45

 - If you have any question or problem, please contact jie.chen@greenwaves-technologies.com for more help