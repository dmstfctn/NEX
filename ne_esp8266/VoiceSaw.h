#include <Arduino.h>
#include "./ADSR.h"

#ifndef _CLASS_VoiceSaw
#define _CLASS_VoiceSaw

class VoiceSaw{
  public:
    VoiceSaw( int len );
    void tick();
    void trigger();
    int phase;
    int cycleLength;
    uint16_t sample;
    ADSR adsr;
  private:
    uint16_t waveMaximum;
    int samp;
    int jump;
};

// constructor
VoiceSaw::VoiceSaw( int len ): 
  adsr() 
{
  cycleLength = len;
  phase = 0;
  waveMaximum = 65534;
  samp = 0;  
  jump = waveMaximum / cycleLength;
  sample = samp * adsr.env;
}

// what to do to get the next sample
void VoiceSaw::tick(){  
  samp += jump;
 
  phase++;
  
  if( phase >= cycleLength ){
    phase = 0;
    samp = 0;
  }
  
  adsr.tick();
  sample = samp * adsr.env;
}

#endif

