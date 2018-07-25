#include <Arduino.h>
#include "./ADSR.h"

#ifndef _CLASS_VoiceTriangle
#define _CLASS_VoiceTriangle

class VoiceTriangle{
  public:
    VoiceTriangle( int len );
    void tick();
    void trigger();
    void setCycle( int len );
    int phase;
    int cycleLength;
    uint16_t sample;
    ADSR adsr;
  private:
    uint16_t waveMaximum;
    int waveMiddle;
    int upCount;
    int crossCount;
    int samp;
    int jump;    
};

// constructor
VoiceTriangle::VoiceTriangle( int len ): 
  adsr() 
{  
  phase = 0;
  waveMaximum = 65534;
  waveMiddle  = waveMaximum / 2;
  
  setCycle( len );
  
  samp = waveMiddle;  
  
  sample = samp * adsr.env;
}

void VoiceTriangle::setCycle( int len ){
  cycleLength = len;
  upCount = cycleLength * 0.25;
  crossCount = cycleLength * 0.5;
  jump = waveMaximum / crossCount;
}

// what to do to get the next sample
void VoiceTriangle::tick(){  
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
  
  adsr.tick();
  sample = samp * adsr.env;
}

#endif
