#include <Arduino.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include "./esp8266.h"

#define WIFI_CH1_F    2412  
#define WIFI_CH2_F    2417  
#define WIFI_CH3_F    2422  
#define WIFI_CH4_F    2427  
#define WIFI_CH5_F    2432  
#define WIFI_CH6_F    2437  
#define WIFI_CH7_F    2442  
#define WIFI_CH8_F    2447  
#define WIFI_CH9_F    2452  
#define WIFI_CH10_F   2457  
#define WIFI_CH11_F   2462  
#define WIFI_CH12_F   2467  
#define WIFI_CH13_F   2472  
#define WIFI_CH14_F   2484 

#define WIFI_CH1_F_LOG10  3.382377
#define WIFI_CH2_F_LOG10  3.383276
#define WIFI_CH3_F_LOG10  3.384174
#define WIFI_CH4_F_LOG10  3.385069
#define WIFI_CH5_F_LOG10  3.385963
#define WIFI_CH6_F_LOG10  3.386855
#define WIFI_CH7_F_LOG10  3.387745
#define WIFI_CH8_F_LOG10  3.388633
#define WIFI_CH9_F_LOG10  3.389520
#define WIFI_CH10_F_LOG10 3.390405
#define WIFI_CH11_F_LOG10 3.391288 
#define WIFI_CH12_F_LOG10 3.392169
#define WIFI_CH13_F_LOG10 3.393048
#define WIFI_CH14_F_LOG10 3.395151

int wifi_calculatePacketDist( int rssi, int wifiChannel ){ 
  // change freqinMHz based on current channel number for better accuracy  
  float freqLog10 = WIFI_CH6_F_LOG10;
  switch( wifiChannel ){
    case 1:
      freqLog10 = WIFI_CH1_F_LOG10;
      break;
    case 2:
      freqLog10 = WIFI_CH2_F_LOG10;
      break;
    case 3:
      freqLog10 = WIFI_CH3_F_LOG10;
      break;
    case 4:
      freqLog10 = WIFI_CH4_F_LOG10;
      break;
    case 5:
      freqLog10 = WIFI_CH5_F_LOG10;
      break;
    case 6:
      freqLog10 = WIFI_CH6_F_LOG10;
      break;
    case 7:
      freqLog10 = WIFI_CH7_F_LOG10;
      break;
    case 8:
      freqLog10 = WIFI_CH8_F_LOG10;
      break;
    case 9:
      freqLog10 = WIFI_CH9_F_LOG10;
      break;
    case 10:
      freqLog10 = WIFI_CH10_F_LOG10;
      break;
    case 11:
      freqLog10 = WIFI_CH11_F_LOG10;
      break;
    case 12:
      freqLog10 = WIFI_CH12_F_LOG10;
      break;
    case 13:
      freqLog10 = WIFI_CH13_F_LOG10;
      break;
    case 14:
      freqLog10 = WIFI_CH14_F_LOG10;
      break;
  }
  // exp is -2.017651591504543 -> 2.995122696531886
  float exp = (27.55 - ( 20.0 * freqLog10 ) + abs( rssi )) / 20.0;
  // so return is 0.009601706100906212 -> 988.8324193470575
  return pow( 10.0, exp );
}
