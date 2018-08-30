#include <Arduino.h>
#include <MozziGuts.h>
#include "./wifi.h"

extern "C" {
  // This includes the base functionality for the ESP8266
  #include "user_interface.h"
}

#include <Oscil.h>
#include <ADSR.h>
#include <tables/square_analogue512_int8.h>
#include <tables/saw_analogue512_int8.h>

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
boolean button0_pressed = false;
boolean button0_released = false;
boolean button1_pressed = false;
boolean button1_released = false;
boolean button0_latch = false;
boolean button1_latch = false;
boolean button0_held = false;
boolean button1_held = false;

long button0_down_at = 0;
long button1_down_at = 0;

long button_hold_threshold = 500000; //half a second;

Oscil<SQUARE_ANALOGUE512_NUM_CELLS, AUDIO_RATE> oscil1( SQUARE_ANALOGUE512_DATA );
Oscil<SAW_ANALOGUE512_NUM_CELLS, AUDIO_RATE> oscil2( SAW_ANALOGUE512_DATA );
ADSR<CONTROL_RATE, AUDIO_RATE> env1;
ADSR<CONTROL_RATE, AUDIO_RATE> env2;

Sample<CLICK_NUM_CELLS, AUDIO_RATE> sound1( CLICK_DATA );
Sample<CLICK_NUM_CELLS, AUDIO_RATE> sound2( CLICK_DATA );
Sample<CLICK_NUM_CELLS, AUDIO_RATE> sound3( CLICK_DATA );

uint8_t currentSound = 0;

// CLICK_BASE_FREQ = ~33
float CLICK_BASE_FREQ = ((float) CLICK_SAMPLERATE / (float) CLICK_NUM_CELLS);


int triggerCount;

void onPacket( uint8_t *buf, uint16_t len ){
  t = mozziMicros(); //instead of millis - mozzi disables this (I think)
  /* calculate type and distance of packet */
  int distance = 10000; // something high that it will never be
  uint8_t type = 0;
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
    sound1.setFreq( CLICK_BASE_FREQ - (distance/100) );
    sound1.start();
  } else if( currentSound == 1 ){
    sound2.setFreq( CLICK_BASE_FREQ - (distance/100) );
    sound2.start();
  } else if( currentSound == 2 ){
    sound3.setFreq( CLICK_BASE_FREQ - (distance/100) );
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


  oscil1.setFreq( 74 ); // ~D1
  oscil2.setFreq( 90 ); // ~F1
  env1.setADLevels( 250, 230 );
  env2.setADLevels( 250, 230 );
  env1.setTimes(100,50,150,300); 
  env2.setTimes(100,50,150,300); 
}

void updateControl(){
  /* UPDATE GLOBAL TIME */
  t = mozziMicros(); //instead of millis - mozzi disables this (I think)

  /* BUTTON CALCULATIONS */
  BTN_0 = digitalRead( 0 );
  BTN_1 = digitalRead( 1 );
  button0_released = false;
  button0_pressed = false;
  button1_released = false;
  button1_pressed = false;
  if( BTN_0 != pBTN_0 ){
    if( BTN_0 == LOW ){ // button 0 pressed
      button0 = true;
      button0_down_at = t;
      button0_pressed = true;
      button0_released = false;
    } else { // button 0 released
      button0 = false;
      button0_held = false;
      button0_latch = !button0_latch;
      button0_released = true;
      button0_pressed = false;
    }
  }
  if( BTN_1 != pBTN_1 ){
    if( BTN_1 == LOW ){ // button 1 pressed
      button1 = true;      
      button1_down_at = t;
      button1_pressed = true;
      button1_released = false;
    } else { // button 1 released
      // nothing
      button1 = false;
      button1_held = false;
      button1_latch = !button1_latch;
      button1_released = true;
      button1_pressed = false;
    }
  }
  if( button0 && t - button0_down_at > button_hold_threshold ){
    button0_held = true;
  }
  if( button1 && t - button1_down_at > button_hold_threshold ){
    button1_held = true;
  }
  pBTN_0 = BTN_0;
  pBTN_1 = BTN_1;

  /* WIFI CHANNEL UPDATES */
  // step through wifi channels  
  if( t - lastChannelChangeAt > channelChangeTimeout ){
    lastChannelChangeAt = t;
    wifiChannel++;

    if( wifiChannel > 13 ){
      wifiChannel = 1;
    }

    wifi_set_channel( wifiChannel );
  }

  /* NORMAL CONTROL STUFF */
  if( button0_held ){
    oscil1.setFreq( 37 );
    env1.noteOn();
  } else if( button0_pressed ){
    oscil1.setFreq( 74 );
    env1.noteOn();
  } else if( button0_released ){
    env1.noteOff();
  }
  if( button1_held ){
    oscil2.setFreq( 45 );
    env2.noteOn();
  } else if( button1_pressed ){
    oscil2.setFreq( 90 );
    env2.noteOn();
  } else if( button0_released ){
    env2.noteOff();
  }

  env1.update();
  env2.update();
}

int updateAudio(){
  int out = (int) sound1.next();
  out +=  (int) sound2.next();
  out +=  (int) sound3.next();

  out += (int) (env1.next() * oscil1.next()) >> 12;
  out += (int) (env2.next() * oscil2.next()) >> 12;

  return out << 8; //the << 8 should map from 8bit int to 16bit int
}

void loop(){
  audioHook();
}

