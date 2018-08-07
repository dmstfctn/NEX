//#define SAMPLE_RATE 16000 
#define SAMPLE_RATE 22050 
//#define SAMPLE_RATE 44100

#include <Arduino.h>

#include "./audio.h"
#include "./wifi.h"

#include "./VoiceTriangle.h"
#include "./VoiceSquare.h"
#include "./VoiceSaw.h"
#include "./VoiceWavetable.h"
#include "./VoiceSample.h"

extern "C" {
  // This includes the base functionality for the ESP8266
  #include "user_interface.h"
}

int wifiChannel = 1;

int lastChannelChangeAt = 0;
int channelChangeTimeout = 5000;

VoiceTriangle triangle1( SAMPLE_RATE / 60 );
VoiceTriangle triangle2( SAMPLE_RATE / 100 );

long lastPktAt = 0;
long pktTimeout = 50;

int voiceIndex = 0;

void onPacket( uint8_t *buf, uint16_t len ){
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

  
  if( type == 1 || type == 2 ){  // triggered by packets that aren't Device Frames 
    if( voiceIndex == 0 ){
      
      if( triangle1.adsr.env <= 0 ){ //retrigger if( complete );
        triangle1.setCycle( SAMPLE_RATE / 60 );
        //triangle1.adsr.triggerEnv( SAMPLE_RATE / 25 );
        triangle1.adsr.triggerEnv( distance * 100 );
      } else { //pitch down gradually
        triangle1.setCycle( triangle1.cycleLength - 1 ); //pitch up
      }
      
      voiceIndex++;
    } else {
       if( triangle2.adsr.env <= 0 ){ //retrigger if( complete );
        triangle2.setCycle( SAMPLE_RATE / 100 );
        //triangle2.adsr.triggerEnv( SAMPLE_RATE / 25 );
        triangle2.adsr.triggerEnv( distance * 100 );
      } else { //pitch down gradually
        triangle2.setCycle( triangle2.cycleLength + 1 ); // pitch down
      }
      voiceIndex = 0;
    }    
  } 
  
}

void setup() {

  delay( 200 );

  //WiFi Setup
  wifi_set_opmode( STATION_MODE ); 
  wifi_set_channel( wifiChannel );  
  wifi_promiscuous_enable( 0 );
  wifi_set_promiscuous_rx_cb( onPacket );
  wifi_promiscuous_enable( 1 );

  delay( 5 );
  
  audio_setup();
  triangle1.adsr.setADSR( SAMPLE_RATE / 50, SAMPLE_RATE / 50, 0.7, SAMPLE_RATE / 50 );
  triangle2.adsr.setADSR( SAMPLE_RATE / 50, SAMPLE_RATE / 50, 0.6, SAMPLE_RATE / 50 );
}

int sampCount = 0;

void loop() {
  yield();
  // if there's space in the audio buffer, add a sample
  if( i2s_available() > 0 ){ 
    triangle1.tick();
    triangle2.tick();
    audio_writeDAC( ( triangle1.sample + triangle2.sample ) / 2 );
  }
  
  yield();
  // step through wifi channels
  long t = millis();
  if( t - lastChannelChangeAt > channelChangeTimeout ){
    lastChannelChangeAt = t;
    wifiChannel++;

    if( wifiChannel > 13 ){
      wifiChannel = 1;
    }

    wifi_set_channel( wifiChannel );
  }
}
