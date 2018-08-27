#include <Arduino.h>
#include <MozziGuts.h>
#include "./wifi.h"

extern "C" {
  // This includes the base functionality for the ESP8266
  #include "user_interface.h"
}

#include <mozzi_midi.h> // for mtof
#include <mozzi_fixmath.h>

#include <Oscil.h>
#include <tables/saw2048_int8.h> // table for Oscils to play
//#include <tables/pinknoise8192_int8.h>
#include <ADSR.h>

#include <ReverbTank.h>
#include <LowPassFilter.h>

unsigned long t;
int wifiChannel = 1;
unsigned long lastChannelChangeAt = 0;
int channelChangeTimeout = 5000000; // in micros

// buttons. LOW is active/pressed (because of pullup stuff)
int BTN_0  = HIGH;
int pBTN_0 = HIGH;
int BTN_1  = HIGH;
int pBTN_1 = HIGH;

boolean button0 = false;
boolean button1 = false;

Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> sound1( SAW2048_DATA );
ADSR <CONTROL_RATE, AUDIO_RATE> env1;
boolean noteOn1 = false;
long noteOnAt1 = 0;

Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> sound2( SAW2048_DATA );
ADSR <CONTROL_RATE, AUDIO_RATE> env2;
boolean noteOn2 = false;
long noteOnAt2 = 0;

Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> sound3( SAW2048_DATA );
ADSR <CONTROL_RATE, AUDIO_RATE> env3;
boolean noteOn3 = false;
long noteOnAt3 = 0;

ReverbTank reverb;
LowPassFilter lowPass;

void onPacket( uint8_t *buf, uint16_t len ){
  t = mozziMicros(); //instead of millis - mozzi disables this (I think)
  /* calculate type and distance of packet */
  double distance = 1000000; // something high that it will never be
  int type = 0;
  if (len < 10) {
    return; // packet is invalid
  } else if( len == 12 ){
    type = 1;
    struct RxControl *sniffer = (struct RxControl*) buf;
    distance = wifi_calculatePacketDist( sniffer->rssi, wifiChannel );
  } else if (len == 128) {  // BEACON FRAME  
    type = 2;
    sniffer_buf2 *data = (sniffer_buf2 *)buf;
    distance = wifi_calculatePacketDist( data->rx_ctrl.rssi, wifiChannel  );    
  } else { //length is X * 10 -> DEVICE FRAME
    type = 3;
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    distance = wifi_calculatePacketDist( sniffer->rx_ctrl.rssi, wifiChannel  );    
  }
  //if( distance < 3 ){
    if( type == 1 ){
      if( !noteOn1 ){
        env1.noteOn();
        noteOnAt1 = t;
        noteOn1 = true;
      }
    } else if( type == 2 ){
      if( !noteOn2 ){
        env2.noteOn();
        noteOnAt2 = t;
        noteOn2 = true;
      }
    } else if( type == 3 ){
      if( !noteOn3 ){
        env3.noteOn();
        noteOnAt3 = t;
        noteOn3 = true;
      }
    }
  //}
 
}

void setup() {
  
  //pin setup
  pinMode(0, INPUT);  // GPIO0 -> BTN_0 
  pinMode(1, FUNCTION_3); // TX -> GPIO1 -> BTN_1
  pinMode(1, INPUT);
  
 
  delay( 200 );

  //WiFi Setup
  wifi_set_opmode( STATION_MODE ); 
  wifi_set_channel( wifiChannel );  
  wifi_promiscuous_enable( 0 );
  wifi_set_promiscuous_rx_cb( onPacket );
  wifi_promiscuous_enable( 1 );

  delay( 5 );
  
  startMozzi( CONTROL_RATE );

  sound1.setFreq( mtof(57.f + 0.f) );
  env1.setADLevels( 240, 50 ); //0 - 255
  env1.setTimes( 25, 25, 25, 25 ); //milliseconds
  env1.update();
  
  sound2.setFreq( mtof(69.f + 0.f) );
  env2.setADLevels( 240, 50 ); //0 - 255
  env2.setTimes( 25, 25, 25, 25 ); //milliseconds
  env2.update();
  
  sound3.setFreq( mtof(81.f + 0.f) );
  env3.setADLevels( 240, 50 ); //0 - 255
  env3.setTimes( 25, 25, 25, 25 ); //milliseconds
  env3.update();

  reverb.setFeebackLevel( 75 );
  lowPass.setCutoffFreq( 25 ); //0 = 0, 255 = AUDIO_RATE/2
  lowPass.setResonance( 30 );
}

void updateControl(){
  t = mozziMicros(); //instead of millis - mozzi disables this (I think)
  BTN_0 = digitalRead( 0 );
  BTN_1 = digitalRead( 1 );

  if( BTN_0 != pBTN_0 ){
    if( BTN_0 == LOW ){ // button 0 pressed
      button0 = true;
      env1.noteOn();
    } else { // button 0 released
      button0 = false;
      env1.noteOff();
    }
  }
  if( BTN_1 != pBTN_1 ){
    if( BTN_1 == LOW ){ // button 1 pressed
      button1 = true;
      env2.noteOn();
    } else { // button 1 released
      // nothing
      button1 = false;
      env2.noteOff();
    }
  }
  // step through wifi channels  
  if( t - lastChannelChangeAt > channelChangeTimeout ){
    lastChannelChangeAt = t;
    wifiChannel++;

    if( wifiChannel > 13 ){
      wifiChannel = 1;
    }

    wifi_set_channel( wifiChannel );
  }

  if( noteOn1 && t - noteOnAt1 > 50000 ){
    if( t - noteOnAt1 > 100000 ){
      noteOn1 = false;
    } else if( t - noteOnAt1 > 50000 ){
      env1.noteOff();    
    }
  }
  if( noteOn2 && t - noteOnAt2 > 50000 ){
    if( t - noteOnAt2 > 100000 ){
      noteOn2 = false;
    } else if( t - noteOnAt2 > 50000 ){
      env2.noteOff();    
    }
  }
  if( noteOn3  && t - noteOnAt3 > 50000 ){
    if( t - noteOnAt3 > 100000 ){
      noteOn3 = false;
    } else if( t - noteOnAt3 > 50000 ){
      env3.noteOff();    
    }
  }

  env1.update();
  env2.update();
  env3.update();
  pBTN_0 = BTN_0;
  pBTN_1 = BTN_1;
}

int updateAudio(){
  int out = ((int) sound1.next() * env1.next() >> 8); 
  out += ((int) sound2.next() * env2.next() >> 8);
  out += ((int) sound3.next() * env3.next() >> 8);
    
  if( button1 ){    
    int rvb = reverb.next( out );
    out = (out + (rvb>>2));        
  } 
  
  if( button0 ){
    out = lowPass.next( out );
  }

  //out = out >> 2; //same as /4 ?

  //return (int16_t) map( out, -128, 127, -32768, 32767 );
  return out << 8; //same as above? (i.e. mapping from 8bit int to 16bit int)
}

void loop(){
  audioHook();
}

