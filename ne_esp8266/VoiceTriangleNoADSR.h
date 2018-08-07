#include <Arduino.h>

#ifndef _CLASS_VoiceTriangleNoADSR
#define _CLASS_VoiceTriangleNoADSR

class VoiceTriangleNoADSR{
  public:
    VoiceTriangleNoADSR( int len );
    void tick();
    void trigger();
    void setCycle( int len );
    int phase;
    int cycleLength;
    uint16_t sample;
    float volume = 1.0;
  private:
    uint16_t waveMaximum;
    int waveMiddle;
    int upCount;
    int crossCount;
    int samp;
    int jump;    
};

// constructor
VoiceTriangleNoADSR::VoiceTriangleNoADSR( int len ){  
  phase = 0;
  waveMaximum = 65534;
  waveMiddle  = waveMaximum / 2;
  
  setCycle( len );
  
  samp = waveMiddle;  
  
  sample = samp * volume;
}

void VoiceTriangleNoADSR::setCycle( int len ){
  cycleLength = len;
  upCount = cycleLength * 0.25;
  crossCount = cycleLength * 0.5;
  jump = waveMaximum / crossCount;
}

// what to do to get the next sample
void VoiceTriangleNoADSR::tick(){  
  if( phase < upCount ){
    samp += jump;
  } else if( phase < upCount + crossCount ){
    samp -= jump;
  } else {
    samp += jump;
  }
  
  phase++;
  
  if( phase >= cycleLength ){
    phase = 0;
  }
  
  sample = samp * volume;
}

#endif
