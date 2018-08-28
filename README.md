# NE4
The NE4 is a small device that collects live data from the wireless networks surrounding it, using this data to trigger or generate sound. It passes the sound directly to a phone via the audio port's microphone in, it's powered through the phone's USB port.

The phone then transmits the sound by means of a phone call, either over the voice network or through 3/4G ensuring that it doesn't interfere with the signals the device picks up.

In this way, the NE4 turns a smartphone into a parasite, piggybacking on its connectivity to enable network exploration, investigation, and performance.

The NE4 was used for [Network Study XV](https://demystification.co/mmittee/network-study-xv/) in April 2018.

The NE4 is built on the ESP8266, a cheap, readily available wifi capable microcontroller. This repository contains the code that it runs, and utilities that help write this.

## ne_esp8266_mozzi
**ne_esp8266_mozzi** is a rewrite of **ne_esp8266** using the Mozzi Library, now that Mozzi's been ported to the ESP8266. It makes use of Mozzi's ```PDM_VIA_I2S``` audio out mode to retain the same circuit as the previous version.

This has to be set in the code of the Mozzi library, at the time of writing this is done in ```AudioConfigESP.h``` on line 14 which should become:

```
#define ESP_AUDIO_OUT_MODE PDM_VIA_I2S
```

More info can be found in the [Mozzi readme](https://github.com/sensorium/Mozzi#esp8266).

It also uses Mozzi's optional higher 32768Hz sample rate, taking advantage of the ESP8266's faster clock speed (80MHz by default, but can be set at 160MHz in the Arduino Tools menu - 10x the original rate Mozzi was written for). This is set by uncommenting line 62 of ```mozzi_config.h```:

```
#define AUDIO_RATE 32768
```

Line 15 of ```AudioConfigESP.h``` can ostensibly be used to get higher output quality by changing the PDM density, although there seems to be some noise introduced by this. 2 is default, but has a high pitched whine around 1KhZ. 16 seems to give an output without this, but could be too taxing for complex operations (it's then 16 * 32 = 512 PDM cycles to output 1 sample so...).

```
#define PDM_RESOLUTION 16
```

There is a fork of Mozzi (as of 27/08/18) at [dmstfctn/Mozzi](https://github.com/dmstfctn/Mozzi/) with this configuration implemented.

## ne_esp8266
**ne_esp8266** contains the code to run on the ESP8266 (we used an ESP-01) to scan for packets and generate sound. It's written as an Arduino sketch, making use of the [ESP8266 Arduino Core](https://github.com/esp8266/Arduino).

Most of the code deals with audio generation and supports Sawtooth, Triangle and Square waves, arbitrary Wavetables and samples as well as an ADSR envelope generator.

```ne_esp8266/audio.h``` sets up and runs the RX pin as an i2s audio output. It owes a lot to the first part of [this post](http://blog.dspsynth.eu/audio-hacking-on-the-esp8266/) by Janost.

```ne_esp8266/esp8266.h``` includes the ESP8266Wifi core functionality and also defines some useful structs for parsing the packets. It owes a lot to [this gist](https://gist.github.com/zgoda/a6854e12d987d727efc68a8fb0860a1c).

```ne_esp8266/wifi.h``` contains a utility function for estimating the distance of a device based on the signal strength of a packet. It's not super accurate but its results are good enough for enabling some basic distance based filtering. There's a bit more info about this on [StackOverflow](https://stackoverflow.com/questions/11217674/how-to-calculate-distance-from-wifi-router-using-signal-strength#11249007)


## util

### util/audio-converter
Converts a .wav file into a uint16_t array and saves it to a .txt file so you can just copy paste it into the .ino sketch. This is written for Node JS, so you'll need that installed to run it. Then, in ```util/audio-converter```, run npm install to grab the dependencies and to run it:

```node main nameofthefile.wav```

This will create ```nameofthefile.wav.txt``` in the same location as the original wav that contains the necessary code to paste int the .ino.

Note that by default the array is called ```sample_wavetable``` and comes with another variable called ```sample_wavetable_length``` containing its length - you'll need to change these if you're using more than one. Not that there's that much space...

See ```sine440.wav``` and ```sine440.wav.txt``` for example input and output.
