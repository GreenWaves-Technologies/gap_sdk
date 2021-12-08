/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#include "dpi/models.hpp"
#include <stdint.h>
#ifdef USE_SNDFILE
#include <sndfile.hh>
#endif



class Microphone;
class Stim;
class Stim_txt;
class I2s_mic_channel;



class Microphone_itf : public I2s_itf
{
public:
  Microphone_itf(Microphone *top) : top(top) {}
  void edge(int64_t timestamp, int sck, int ws, int sd);

private:
    Microphone *top;
};



class Microphone : public Dpi_model
{
  friend class Stim;
  friend class Stim_txt;
  friend class I2s_mic_channel;

public:
  Microphone(js::config *config, void *handle);

  void start();
  void edge(int64_t timestamp, int sck, int ws, int sd);

private:

  void setData(int64_t timestamp, int sck, int ws);
  void setData(int64_t timestamp, int channel, int sck, int ws);
  void clrData(int64_t timestamp, int channel);

  I2s_itf *itf;
  int width;
  std::string stimLeftPath;
  std::string stimRightPath;
  int prevSck;
  int prevWs;
  int currentChannel;
  I2s_mic_channel *channels[2];
  bool pdm;
  bool ddr;
  bool dual;
  int freq;
  int flush_data;
  int chain_size;
  std::vector<I2s_mic_channel *> channel_chain;

  void *trace;

  int current_bit;
  int current_channel;
};


class Stim {

public:
  virtual long long getData(int64_t timestamp) = 0;

};

class Stim_txt : public Stim {

public:
  Stim_txt(Microphone *top, void *handle, std::string file, int width, int freq, bool raw=false, bool useLibsnd=false);
  long long getData(int64_t timestamp);
  long long getDataFromFile();

private:
  Microphone *top;
  int width;
  FILE *stimFile;
  std::string filePath;
  int period;
  int64_t lastDataTime;
  long long lastData;
  int64_t nextDataTime;
  long long nextData;
  bool raw;
  bool useLibsnd;
#ifdef USE_SNDFILE
  SndfileHandle sndfile;
#endif
};


class I2s_mic_channel {

public:
  I2s_mic_channel(int id, Microphone *top, void *handle, int width, std::string stimFile, bool pdm, int freq);
  int popData(int64_t timestamp);
  void clrData(int64_t timestamp);

private:
  Microphone *top;
  bool pdm;
  bool mic;
  int width;
  int pendingBits;
  Stim *stim;
  unsigned long long currentValue;
  long long pdmError;
  int id;
};



Stim_txt::Stim_txt(Microphone *top, void *handle, std::string file, int width, int freq, bool raw, bool useLibsnd)
: top(top), width(width), filePath(file), raw(raw), useLibsnd(useLibsnd)
{
  if (useLibsnd) {

#ifdef USE_SNDFILE

    unsigned int pcm_width = width == 16 ? SF_FORMAT_PCM_16 : SF_FORMAT_PCM_32;
    sndfile = SndfileHandle (file, SFM_READ, SF_FORMAT_WAV | pcm_width) ;
    freq = sndfile.samplerate ();

#else

    top->fatal("Unable to open file (%s), libsndfile support is not active\n", file.c_str());
    return;

#endif

  } else {
    stimFile = fopen(file.c_str(), "r");
    if (stimFile == NULL) {
      this->top->fatal("\033[1m\033[31mFailed to open stimuli file\033[0m: %s: %s", file.c_str(), strerror(errno));
    }
  }
  if (freq) period = 1000000000000UL / freq;
  else period = 0;

  lastDataTime = -1;
  nextDataTime = -1;
}

static inline int getSignedValue(unsigned long long val, int bits)
{
  return ((int)val) << (64-bits) >> (64-bits);
}

long long Stim_txt::getDataFromFile()
{
  if (useLibsnd) {

#ifdef USE_SNDFILE

    int32_t result;
    if (this->width <= 16)
    {
      int16_t sample;
      sndfile.read (&sample, 1);
      result = (int32_t)sample;
    }
    else if (this->width <= 32)
    {
      int32_t sample;
      sndfile.read (&sample, 1);
      result = (int32_t)sample;
    }

    return result;
#else
    return 0;
#endif

  } else if (raw) {
    unsigned long long data = 0;
    while(fread((void *)&data, 2, 1, stimFile) != 1)
    {
      fclose(stimFile);
      stimFile = fopen(filePath.c_str(), "r");    
    }

    long long result = getSignedValue(data, width);

    this->top->trace_msg(this->top->trace, 4, "Got new sample1 (value: 0x%x)", result);
    return result;
  } else {
    char *line = NULL;
    size_t len = 0;
    while(::getline(&line, &len, stimFile) == -1) {
      fclose(stimFile);
      stimFile = fopen(filePath.c_str(), "r");    
    }
  
    unsigned long long data = strtol(line, NULL, 16);
    long long result = getSignedValue(data, width);

    this->top->trace_msg(this->top->trace, 4, "Got new sample2 (value: 0x%x)", result);
  
    return result;
  }
}

long long Stim_txt::getData(int64_t timestamp)
{
  if (period == 0) return getDataFromFile();

  if (lastDataTime == -1) {
    lastData = getDataFromFile();
    lastDataTime = timestamp;
  }

  if (nextDataTime == -1) {
    nextData = getDataFromFile();
    nextDataTime = lastDataTime + period;
  }

  // Get samples from the file until the current timestamp fits the window
  while (timestamp >= nextDataTime) {
    lastDataTime = nextDataTime;
    lastData = nextData;
    nextDataTime = lastDataTime + period;
    nextData = getDataFromFile();
  }

  // Now do the interpolation between the 2 known samples
  float coeff = (float)(timestamp - lastDataTime) / (nextDataTime - lastDataTime);
  float value = (float)lastData + (float)(nextData - lastData) * coeff;


  top->trace_msg(this->top->trace, 4, "Interpolated new sample (value: %d, timestamp: %ld, prev_timestamp: %ld, next_timestamp: %ld, prev_value: %d, next_value: %d)", value, timestamp, lastDataTime, nextDataTime, lastData, nextData);
  

  //printf("%f %f %d %d %ld %ld %ld\n", coeff, value, lastData, nextData, lastDataTime, timestamp, nextDataTime);

  return (int)value;
}


I2s_mic_channel::I2s_mic_channel(int id, Microphone *top, void *handle, int width, std::string stimFile, bool pdm, int freq)
 : width(width), pendingBits(0), stim(NULL), pdm(pdm), pdmError(0), id(id)
{
  if (stimFile != "") {

    char *ext = rindex((char *)stimFile.c_str(), '.');

    if (ext == NULL) {
      top->print("\033[1m\033[31mUnsupported file extension\033[0m  : %s", stimFile.c_str());
      return;
    }

    if (strcmp(ext, ".hex") == 0) {
      stim = new Stim_txt(top, handle, stimFile, width, freq);
    } else if (strcmp(ext, ".raw") == 0) {
      stim = new Stim_txt(top, handle, stimFile, width, freq, true);
    } else if (strcmp(ext, ".wav") == 0) {
      stim = new Stim_txt(top, handle, stimFile, width, freq, false, true);
    } else {
      top->print("\033[1m\033[31mUnsupported file extension\033[0m  : %s", stimFile.c_str());
    }
  }
}


void I2s_mic_channel::clrData(int64_t timestamp)
{
  pendingBits = 0;
}

int I2s_mic_channel::popData(int64_t timestamp)
{

  if (!stim) return 0;

  if (pdm) {

    // PDM mode, only transmit one modulated bit per sample

    long long sample = stim->getData(timestamp);
    unsigned long long value = sample + (1 << (width - 1));

    unsigned long long maxVal = (1 << (width)) - 1;

    pdmError += value;

    if (pdmError >= maxVal) {
      pdmError -= maxVal;
      return 1;
    } else {
      return 0;
    }

  } else {

    // Classic mode, sampled one full value and stream one bit per cycle

    // Sample new data if all bits have been shifted

    if (pendingBits == 0) {
      pendingBits = width;
      if (stim) {
        currentValue = stim->getData(timestamp);
      }
      else currentValue = 0;
    }

    // Shift bits from MSB
    int bit = (currentValue >> (pendingBits - 1)) & 1;
    pendingBits--;
  
    return bit;
  }
}



void Microphone::setData(int64_t timestamp, int channel, int sck, int ws)
{
  if (this->chain_size > 1)
  {
    int val = this->channel_chain[channel]->popData(timestamp);
    itf->rx_edge(sck, ws, val);
  }
  else
  {
    int val = channels[channel]->popData(timestamp);
    itf->rx_edge(sck, ws, val);
  }
}

void Microphone::clrData(int64_t timestamp, int channel)
{
  if (this->chain_size > 1)
  {
    this->channel_chain[channel]->clrData(timestamp);
  }
  else
  {
    channels[channel]->clrData(timestamp);
  }
}

void Microphone::setData(int64_t timestamp, int sck, int ws)
{
  setData(timestamp, currentChannel, sck, ws);
}


void Microphone::edge(int64_t timestamp, int sck, int ws, int sd)
{
  this->trace_msg(this->trace, 4, "Edge (sck: %d, ws: %d)", sck, ws);

  if (ddr) {

    // DOUBLE DATA RATE
    // We ignore WS and send a data at each edge
    if (prevSck == 0 && sck == 1) {
      // Rising edge, prepare data from second microphone so that it is sampled during th next falling edge
      setData(timestamp, 1, sck, 0);
    } else {
      if (flush_data >= 0)
      {
        flush_data--;
        if (flush_data == -1)
        {
          clrData(timestamp, 0);
          clrData(timestamp, 1);
        }
      }

      // Falling edge, prepare data from first microphone so that it is sampled during th next raising edge
      setData(timestamp, 0, sck, 0);
    }

  } else {

    if (this->chain_size > 1)
    {
      // SINGLE DATA RATE
      if (prevSck == 1 && sck == 0) {

        // Falling edge, update data
        setData(timestamp, sck, ws);

        if (flush_data >= 0)
        {
          flush_data--;
          if (flush_data == 0)
          {
            flush_data = this->width;
            currentChannel++;
            if (currentChannel == this->chain_size)
              currentChannel = 0;

            clrData(timestamp, 0);
          }
        }
        
      } else if (prevSck == 0 && sck == 1) {
        if (ws && prevWs != ws)
        {
          currentChannel = 0;
          flush_data = this->width;
            clrData(timestamp, 0);
        }
        prevWs = ws;

      }
    }
    else
    {
      // SINGLE DATA RATE
      if (prevSck == 1 && sck == 0) {
        if (flush_data >= 0)
        {
          flush_data--;
          if (flush_data == -1)
          {
            clrData(timestamp, 0);
          }
        }

        // Falling edge, update data
        setData(timestamp, sck, ws);
      } else if (prevSck == 0 && sck == 1) {
      
        if (prevWs != ws) {
          if (this->dual)
          {
            clrData(timestamp, currentChannel);
            flush_data = 0;
            currentChannel = ws;
            prevWs = ws;
          }
          else
          {
            clrData(timestamp, currentChannel);
            flush_data = 0;
            currentChannel = 0;
            prevWs = ws;
          }
        }
      }
    }
  }

  prevSck = sck; 
}

void Microphone_itf::edge(int64_t timestamp, int sck, int ws, int sd)
{
  this->top->edge(timestamp, sck, ws, sd);
}

Microphone::Microphone(js::config *config, void *handle)
: Dpi_model(config, handle), currentChannel(0), prevSck(0), prevWs(0), freq(0),
flush_data(-1)
{
  itf = new Microphone_itf(this);
  create_itf("i2s", static_cast<I2s_itf *>(itf));

  this->width = config->get_child_int("width");
  this->pdm = config->get_child_bool("pdm");
  this->ddr = config->get_child_bool("ddr");
  this->dual = config->get_child_bool("dual");
  this->freq = config->get_child_int("frequency");
  this->chain_size = config->get_child_int("chain_size");

  this->trace = this->trace_new(config->get_child_str("name").c_str());

  if (this->chain_size <= 1)
  {
    this->stimLeftPath = config->get_child_str("stim_left");
    this->stimRightPath = config->get_child_str("stim_right");

    this->print("Instantiated I2S microphone model (i2s_microphone) (width: %d, stimLeft: %s, stimRight: %s)", this->width, this->stimLeftPath.c_str(), this->stimRightPath.c_str());

    this->channels[0] = new I2s_mic_channel(0, this, handle, width, stimLeftPath, pdm, freq);
    if (this->ddr || this->dual) this->channels[1] = new I2s_mic_channel(1, this, handle, width, stimRightPath, pdm, freq);
  }
  else
  {
    this->print("Instantiated I2S microphone chain model (i2s_microphone) (chain size: %d, width: %d)", this->chain_size, this->width);

    this->channel_chain.reserve(this->chain_size);

    for (int i=0; i<this->chain_size; i++)
    {
      std::string stim_path = config->get_child_str("stim_" + std::to_string(i));
      this->print("Instantiated I2S microphone model (i2s_microphone) (width: %d, stim: %s)", this->width, stim_path.c_str());
      this->channel_chain[i] = new I2s_mic_channel(i, this, handle, width, stim_path, false, freq);
    }
  }

  this->current_bit = 0;
  this->current_channel = 0;
}



void Microphone::start()
{
}



extern "C" Dpi_model *dpi_model_new(js::config *config, void *handle)
{
  return new Microphone(config, handle);
}
