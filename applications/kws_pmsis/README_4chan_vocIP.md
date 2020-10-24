# I2S one shot audio stereo 4 microphones plugged with vocal IP

Please read first README.md to setup properly your board and basic instructions

Do
~~~~sh
git checkout master
git pull
~~~~
in sdk_gap and sdk_gap/mbed-os

##Introduction
This test does realtime audio acquisition on 4 mics until a full buffer of 1s audio recording output from vocalIP is completed.

I2S0 and I2S1 so 4 channels coming from 4 mics are recorded.
Each mic is recorded in a 128 samples of 16bits buffer.
There is a double buffer for each I2S ensuring the real time.

Each time a couple of buffer recording I2S0 and I2S1 is full, vocalIP is called and its output save in the 1s buffer.
This buffer will be used by example as the input of a KWS demo.

Running the C code also run a python script that awaits the C code completion to download the buffer in .wav file and play it in VLC.

##Program setup

Pump up the volume of your PC to the max, take care to your ears !
You need for the first time compilation in a directory:
~~~~sh
make clean all
~~~~

There are few switch in test_I2S.c
~~~~c
#define VOCAL 1
#define PERF 1
#define MONITOR_VOCALIP_RES//for debug
//#define MONITOR_FC_ALIVE//for debug
~~~~

Uncommented VOCAL line means that vocalIP is used. Its output is saved in buffer to be saved in .wav at the end of the execution.
Comment it, means the I2Sx data stream is recorded in a whole buffer then saved in .wav. The first buffer for I2S0 is played by VLC. For the second one you need to run VLC manually on the 2nd .wav file.

PERF is only used if VOCAL line is uncommented.
Uncommented PERF line means that perf wrapping the vocalIP routine are recorded and displayed at the end of the execution.
Comment PERF line, no performance is recorded and so nothing displayed.

Uncommented MONITOR_VOCALIP_RES could break the realtime recording as printf is long to execute but give visibility if vocalIP as detected a signal. It's usage is only in a configuration when the program crash.
Comment MONITOR_VOCALIP_RES should be the main case to avoid to impact realtime.

Uncommented MONITOR_FC_ALIVE to have a printf after x main while(1) loop execution. Its purpose is to show that the program is not crashed and shows few statistics. It could breaks the realtime recording as printf is long to execute. It is used for debug purpose only.
Comment MONITOR_FC_ALIVE should be the main case to avoid to impact realtime.

##Examples of how to run with various setup

Notice that all these run have been done with these 2 lines like this:
~~~~c
    //if ((NB_CHUNK) == cntRes)//this should be the line to use
    if ((NB_CHUNK) == cntC) //could allow program to finish
while it should be:
    if ((NB_CHUNK) == cntRes)//this should be the line to use
    //if ((NB_CHUNK) == cntC) //could allow program to finish
~~~~
But then even without any sound simulation or too low sound level simulation, II) crashes.

###I) run with vocalIP, high volume sound record
ensure this line is uncommented:
~~~~c
#define VOCAL 1
~~~~

~~~~sh
make all run
~~~~

In another window launch a sound to test vocal IP
~~~~sh
play -n  synth 5 sin 880 vol 0.5  2>/dev/null
~~~~
then press immediately return in the shell window that execute the test = where 'make all run' has been done.

Or you can speak to the microphone instead of the play command.
It is apparently needed to speak very strongly to have a chance to have sound recognise by vocal IP with the sensorboard. This play command ensure vocalIP detection.

=> the sw crash, the python script waits in vain the c code to tell it is ok to download the record in .wav

###Ib) run with vocalIP, high volume sound record with MONITOR_VOCALIP_RES uncommented
This is to have a bit more informations in which condition the program crashes. It likely makes the audio acquisition no more in realtime.
~~~~c
#define VOCAL 1
#define MONITOR_VOCALIP_RES
~~~~

~~~~sh
make all run
~~~~

In another window launch a sound to test vocal IP:
~~~~sh
play -n  synth 5 sin 880 vol 0.5  2>/dev/null
~~~~
then press immediately return in the shell window that execute the test = where 'make all run' has been done.
=> the sw crashes and depending of the execution few call to vocalIP could have been done.
Example of screen output showing a crash after 4 call to vocalIP:
~~~~sh
Starting execution
py: wait c code to start
c: Ready to record audio from microphones
py: Press return to start sound acquisition:
*********************
Start recording audio
*********************
after prepI2sRx
	nbIT 33 33 cntC 31 cntRes 0
	nbIT 35 35 cntC 32 cntRes 1
	nbIT 39 39 cntC 33 cntRes 2
	nbIT 42 42 cntC 34 cntRes 3
py: waiting recReady
py: waiting recReady
py: waiting recReady
py: waiting recReady
~~~~
And the python lines with go on for ever...

###II) run with vocalIP, low volume sound record
Ensure this line is uncommented:
~~~~c
#define VOCAL 1
~~~~
~~~~sh
make all run
~~~~

in another window launch a sound to test vocal IP:
~~~~sh
play -n  synth 5 sin 347 vol 0.5  2>/dev/null
~~~~
then press immediately return in the shell window that execute the test = where 'make all run' has been done.

Or you can speak to the microphone instead of the play command.
It is apparently needed to speak very strongly to have a chance to have sound recognise by vocal IP with the sensorboard. This play command ensure vocalIP detection.

=> the sw finish
The performance measured to execute the routine wrapping vocalIP depends of the version of the SDK. On the one you should have from our website and by doing
~~~~sh
git checkout master
git pull
~~~~
it should takes less than 45k cycles (depends of the commit of the gap_sdk and mbed-os).
The sound emitted is really low in dB compare to I)

Check the generate .wav generating, here no sound recorded:
~~~~sh
od -cb BUILD/GAP8/GCC_RISCV/wavOutVoc.wav |less
~~~~

###III) Audio recording to demonstrate audio 4 mics acquisition is functional
In order to avoid any memory overflow as the 4 mics input are recorded, only 32786 samples by micro are recorded.

comment in test_I2S.c:
~~~~c
//#define VOCAL 1
~~~~
=> the I2S datastream is recopied respectively in vocalOutput buffer for I2S0 and vocalOutput2 for I2S1.

vocalOutput is recorded in wavOutVoc.wav
vocalOutput2 is recorded in wavOutVoc2.wav
This time the automatic launch of vlc play a sound recorded in wavOutVoc.wav
You have to play manually the I2S1 recorded by
~~~~sh
vlc wavOutVoc2.wav
~~~~




