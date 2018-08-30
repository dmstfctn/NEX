#include <Arduino.h>
#include <MozziGuts.h>
#include "./wifi.h"

extern "C" {
  // This includes the base functionality for the ESP8266
  #include "user_interface.h"
}

#include <Sample.h>
#include "./samples/CLICK_SR32768.h"

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
boolean button0_latch = false;
boolean button1_latch = false;

Sample<CLICK_NUM_CELLS, AUDIO_RATE> sound1( CLICK_DATA );
Sample<CLICK_NUM_CELLS, AUDIO_RATE> sound2( CLICK_DATA );
Sample<CLICK_NUM_CELLS, AUDIO_RATE> sound3( CLICK_DATA );

uint8_t currentSound = 0;
int CLICK_BASE_FREQ = ((float) CLICK_SAMPLERATE / (float) CLICK_NUM_CELLS);


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

  if( currentSound == 0 ){
    sound1.setFreq( CLICK_BASE_FREQ - (float) distance );
    sound1.start();
  } else if( currentSound == 1 ){
    sound2.setFreq( CLICK_BASE_FREQ - (float) distance );
    sound2.start();
  } else if( currentSound == 2 ){
    sound3.setFreq( CLICK_BASE_FREQ - (float) distance );
    sound3.start();
  }

  currentSound++;
  if( currentSound > 2 ){
    currentSound = 0;
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
  
  startMozzi( CONTROL_RATE );
  sound1.setFreq( CLICK_BASE_FREQ );
  sound2.setFreq( CLICK_BASE_FREQ );
  sound3.setFreq( CLICK_BASE_FREQ );

}

void updateControl(){
  t = mozziMicros(); //instead of millis - mozzi disables this (I think)
  BTN_0 = digitalRead( 0 );
  BTN_1 = digitalRead( 1 );

  if( BTN_0 != pBTN_0 ){
    if( BTN_0 == LOW ){ // button 0 pressed
      button0 = true;
    } else { // button 0 released
      button0 = false;
      button0_latch = !button0_latch;
    }
  }
  if( BTN_1 != pBTN_1 ){
    if( BTN_1 == LOW ){ // button 1 pressed
      button1 = true;      
    } else { // button 1 released
      // nothing
      button1 = false;
      button1_latch = !button1_latch;
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

  pBTN_0 = BTN_0;
  pBTN_1 = BTN_1;
}

int updateAudio(){
  int out = (int) sound1.next();
  out +=  (int) sound2.next();
  out +=  (int) sound3.next();

  return out << 8; //the << 8 should map from 8bit int to 16bit int
}

void loop(){
  audioHook();
}

