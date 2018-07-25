#include <Arduino.h>
#include "./ADSR.h"

#ifndef _CLASS_VoiceSample
#define _CLASS_VoiceSample

class VoiceSample{
  public:
    VoiceSample( const uint16_t* table, int len );
    void tick();
    void trigger();
    void setSpeed( float speed );
    void setVolume( float vol );
    int phase;
    int samplePos;
    int sampleStart;
    int sampleEnd;
    int sampleDir;
    float sampleSpeed;
    int sampleJump;
    int sampleLength;
    int wavetableLength;
    uint16_t sample;
    boolean useADSR = false;
    float volume = 1.0;
    ADSR adsr;
  private:
    int samp;
    int INT_CALC_SCALE = 10000;
    const uint16_t* wavetable;
};

// constructor
VoiceSample::VoiceSample( const uint16_t* table, int len ): 
  adsr() 
{
  wavetable = table;
  wavetableLength = len;
  phase = 0;
  samplePos = phase * INT_CALC_SCALE;
  sampleLength = wavetableLength * INT_CALC_SCALE;
  sampleStart = 0 * INT_CALC_SCALE;
  sampleEnd = sampleLength;
  sampleDir = 1;
  sampleSpeed = 1.0;
  sampleJump = ((float) sampleDir * sampleSpeed) * INT_CALC_SCALE;
  samp = 0;  

  sample = samp * adsr.env;
}

void VoiceSample::setSpeed( float speed ){
  sampleSpeed = speed;
  sampleJump = ((float) sampleDir * sampleSpeed) * INT_CALC_SCALE;
}

void VoiceSample::setVolume( float vol ){
  if( vol < 0 ){
    vol = 0;
  }
  if( vol > 1.0 ){
    vol = 1.0;
  }
  volume = vol; 
}

// what to do to get the next sample
void VoiceSample::tick(){  
  samp = wavetable[ phase ];  
  
  samplePos += sampleJump;
  
  if( samplePos >= sampleEnd ){
    samplePos = sampleStart;
  } else if( phase < sampleStart ){
    samplePos = sampleEnd;
  }

  phase = samplePos / INT_CALC_SCALE;
  if( useADSR ){
    adsr.tick();
    sample = (samp * adsr.env) * volume;
  } else {
    sample = samp * volume;
  }
}

#endif

