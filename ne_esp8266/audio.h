#include <i2s.h>
#include <i2s_reg.h>

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 44100
#endif

void audio_setup(){
  i2s_begin();
  i2s_set_rate( SAMPLE_RATE );
}


uint32_t i2sACC;
uint16_t DAC = 0x8000;
uint16_t err;

void audio_writeDAC(uint16_t DAC) {
  for( uint8_t i = 0; i < 32; i++ ){
    i2sACC = i2sACC<<1;
    if( DAC >= err ) {
      i2sACC|=1;
      err += 0xFFFF - DAC;
    } else {
      err -= DAC;
    }
  }
  bool flag = i2s_write_sample( i2sACC );
}
