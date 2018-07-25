#NE4
The NE4 is a small device that collects live data from the wireless networks surrounding it, using this data to trigger or generate sound. It passes the sound directly to a phone via the audio port's microphone in, it's powered through the phone's USB port. 

The phone then transmits the sound by means of a phone call, either over the voice network or through 3/4G ensuring that it doesn't interfere with the signals the device picks up.

In this way, the NE4 turns a smartphone into a parasite, piggybacking on its connectivity to enable network exploration, investigation, and performance.

The NE4 was used as part of [this performance](https://demystification.co/mmittee/network-study-xv/) in April 2018.

The NE4 is built on the ESP8266, a cheap, readily available wifi capable microcontroller. This repository contains the code that it runs, and utilities that help write this.

##ne_esp8266
**ne_esp8266** contains the code to run on the ESP8266 (we used an ESP-01) to scan for packets and generate sound. It's written as an Arduino sketch, making use of the [ESP8266 Arduino Core](https://github.com/esp8266/Arduino).

Most of the code deals with audio generation and supports Sawtooth, Triangle and Square waves, arbitrary Wavetables and samples as well as an ADSR envelope generator.

```audio.h``` sets up and runs the RX pin as an i2s audio output. It owes a lot to the first part of [this post](http://blog.dspsynth.eu/audio-hacking-on-the-esp8266/) by Janost.

```esp8266.h``` includes the ESP8266Wifi core functionality and also defines some useful structs for parsing the packets. It owes a lot to [this gist](https://gist.github.com/zgoda/a6854e12d987d727efc68a8fb0860a1c).

```wifi.h``` contains a utility function for estimating the distance of a device based on the signal strength of a packet. It's not super accurate but its results are good enough for enabling some basic distance based filtering. There's a bit more info about this on [StackOverflow](https://stackoverflow.com/questions/11217674/how-to-calculate-distance-from-wifi-router-using-signal-strength#11249007)


##util

###util/audio-converter
Converts a .wav file into a uint16_t array and saves it to a .txt file so you can just copy paste it into the .ino sketch. This is written for Node JS, so you'll need that installed to run it. Then, in ```util/audio-converter```, run npm install to grab the dependencies and to run it:

```node main nameofthefile.wav```

This will create ```nameofthefile.wav.txt``` in the same location as the original wav that contains the necessary code to paste int the .ino.

Note that by default the array is called ```sample_wavetable``` and comes with another variable called ```sample_wavetable_length``` containing its length - you'll need to change these if you're using more than one. Not that there's that much space...

See ```sine440.wav``` and ```sine440.wav.txt``` for example input and output.