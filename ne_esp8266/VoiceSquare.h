#include <Arduino.h>
#include "./ADSR.h"

#ifndef _CLASS_VoiceSquare
#define _CLASS_VoiceSquare

class VoiceSquare{
  public:
    VoiceSquare( int len );
    void tick();
    void trigger();
    int phase;
    int cycleLength;
    uint16_t sample;
    ADSR adsr;
  private:
    uint16_t waveMaximum;    
    int cycleHalf;
    int samp;
};

// constructor
VoiceSquare::VoiceSquare( int len ): 
  adsr() 
{
  cycleLength = len;
  phase = 0;
  waveMaximum = 65534;
  cycleHalf = len / 2;
  samp = 0;  

  sample = samp * adsr.env;
}

// what to do to get the next sample
void VoiceSquare::tick(){  
  if( phase < cycleHalf ){
    samp = 0;
  } else {
    samp = waveMaximum;
  }
  
  phase++;
  
  if( phase >= cycleLength ){
    phase = 0;
  }
  
  adsr.tick();
  sample = samp * adsr.env;
}

#endif

