#include <Arduino.h>
#include "./ADSR.h"

#ifndef _CLASS_VoiceSampleOneShot
#define _CLASS_VoiceSampleOneShot

class VoiceSampleOneShot{
  public:
    VoiceSampleOneShot( const uint16_t* table, int len );
    void tick();
    void setSpeed( float speed );
    void setVolume( float vol );
    void trigger();
    void cancel();
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
    boolean triggered = false;
  private:    
    int samp;
    int INT_CALC_SCALE = 10000;
    const uint16_t* wavetable;
};

// constructor
VoiceSampleOneShot::VoiceSampleOneShot( const uint16_t* table, int len ): 
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

void VoiceSampleOneShot::setSpeed( float speed ){
  sampleSpeed = speed;
  sampleJump = ((float) sampleDir * sampleSpeed) * INT_CALC_SCALE;
}

void VoiceSampleOneShot::setVolume( float vol ){
  if( vol < 0 ){
    vol = 0;
  }
  if( vol > 1.0 ){
    vol = 1.0;
  }
  volume = vol; 
}

void VoiceSampleOneShot::trigger(){
  triggered = true;
}
void VoiceSampleOneShot::cancel(){
  triggered = false;
}

// what to do to get the next sample
void VoiceSampleOneShot::tick(){  
  samp = wavetable[ phase ];  
  if( triggered ){
    samplePos += sampleJump;
  }
  
  if( samplePos >= sampleEnd ){
    triggered = false;
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
  if( !triggered ){
    sample = 32767;
  }
}

#endif

