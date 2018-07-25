#include <Arduino.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include "./esp8266.h"

double wifi_calculatePacketDist( double rssi, int wifiChannel ){ 
  double freqInMHz = 2400;
  // change freqinMHz based on current channel number for better accuracy  
  if( wifiChannel == 14 ){
    freqInMHz = 2484;
  } else {
    freqInMHz += 12 + ( (wifiChannel-1) * 5 );
  }
  double exp = (27.55 - (20.0 * log10( freqInMHz )) + abs( rssi )) / 20.0;
  return pow( 10.0, exp );
}
