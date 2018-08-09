// can use other sample rates - try 44100 for higher 
// quality or 16000 to allow for some more complex processing
#define SAMPLE_RATE 22050 

#include <Arduino.h>

#include "./audio.h"
#include "./wifi.h"

#include "./VoiceTriangle.h"
#include "./VoiceSquare.h"
#include "./VoiceSaw.h"

extern "C" {
  // This includes the base functionality for the ESP8266
  #include "user_interface.h"
}

long t;

int wifiChannel = 1;

int lastChannelChangeAt = 0;
int channelChangeTimeout = 5000;

// buttons. LOW is active/pressed (because of pullup stuff)
int BTN_0  = HIGH;
int pBTN_0 = HIGH;
int BTN_1  = HIGH;
int pBTN_1 = HIGH;

VoiceSquare sound1( SAMPLE_RATE / 220 );
VoiceSquare sound2( SAMPLE_RATE / 440 );
VoiceSquare sound3( SAMPLE_RATE / 880 );

VoiceTriangle sound4( SAMPLE_RATE / 55 );
VoiceSaw sound5( SAMPLE_RATE / 110 );

void onPacket( uint8_t *buf, uint16_t len ){
  t = millis();
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


  if( type == 1 ){
    sound1.adsr.triggerEnv();
  } else if( type == 2 ){
    sound2.adsr.triggerEnv();
  } else if( type == 3 ){
    sound3.adsr.triggerEnv();
  }
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
  
  audio_setup();
  sound1.adsr.setADSR( SAMPLE_RATE / 100, SAMPLE_RATE / 100, 0.8, SAMPLE_RATE / 100 );
  sound2.adsr.setADSR( SAMPLE_RATE / 100, SAMPLE_RATE / 100, 0.8, SAMPLE_RATE / 100 );
  sound3.adsr.setADSR( SAMPLE_RATE / 100, SAMPLE_RATE / 100, 0.8, SAMPLE_RATE / 100 );
  
  sound4.adsr.setADSR( SAMPLE_RATE / 4, SAMPLE_RATE / 10, 0.9, SAMPLE_RATE );
  sound5.adsr.setADSR( SAMPLE_RATE / 4, SAMPLE_RATE / 10, 0.9, SAMPLE_RATE );

}

int sampCount = 0;

void loop() {
  t = millis();

  BTN_0 = digitalRead( 0 );
  BTN_1 = digitalRead( 1 );

  if( BTN_0 != pBTN_0 ){
    if( BTN_0 == LOW ){
      sound4.adsr.startEnv();
    } else {
      sound4.adsr.releaseEnv();
    }
  }
  if( BTN_1 != pBTN_1 ){
    if( BTN_1 == LOW ){
      sound5.adsr.startEnv();
    } else {
      sound5.adsr.releaseEnv();
    }
  }
  
  yield();
  // if there's space in the audio buffer, add a sample
  if( i2s_available() > 0 ){ 
    sound1.tick();
    sound2.tick();
    sound3.tick();

    sound4.tick();
    sound5.tick();
   
    //audio_writeDAC( ((sound1.sample + sound2.sample + sound3.sample) /10 )+ ((sound4.sample + sound5.sample) / 2) );
    audio_writeDAC( (sound4.sample + sound5.sample) / 2);
  }
  
  yield();
  
  // step through wifi channels  
  if( t - lastChannelChangeAt > channelChangeTimeout ){
    lastChannelChangeAt = t;
    wifiChannel++;

    if( wifiChannel > 13 ){
      wifiChannel = 1;
    }

    wifi_set_channel( wifiChannel );
  }

  pBTN_0 = BTN_0;
  pBTN_1 = BTN_1;
}
