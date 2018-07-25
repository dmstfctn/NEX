#include <Arduino.h>
#include "./ADSR.h"

#ifndef _CLASS_VoiceWavetable
#define _CLASS_VoiceWavetable

class VoiceWavetable{
  public:
    VoiceWavetable( const uint16_t* table, int lenh );
    void tick();
    void trigger();
    int phase;
    int wavetableLength;
    uint16_t sample;
    ADSR adsr;
  private:
    int samp;
    const uint16_t* wavetable;
};

// constructor
VoiceWavetable::VoiceWavetable( const uint16_t* table, int len ): 
  adsr() 
{
  wavetable = table;
  wavetableLength = len;
  phase = 0;
  samp = 0;  

  sample = samp * adsr.env;
}

// what to do to get the next sample
void VoiceWavetable::tick(){  
  samp = wavetable[ phase ];  
  
  phase++;
  
  if( phase >= wavetableLength ){
    phase = 0;
  }
  
  adsr.tick();
  sample = samp * adsr.env;
}

#endif

