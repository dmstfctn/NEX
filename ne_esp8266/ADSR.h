// some parts from: http://www.earlevel.com/main/2013/06/03/envelope-generators-adsr-code/
#include <Arduino.h>

#ifndef _CLASS_ADSR
#define _CLASS_ADSR

class ADSR {
  public:
    ADSR();
    void startEnv();
    void releaseEnv();
    void triggerEnv();
    void triggerEnv( int sSamp );
    float tick();
    void setAttack( int aSamp );
    void setDecay( int dSamp );
    void setSustain(float sLvl );
    void setSustainTime( int sSamp );
    void setRelease( int rSamp );
    void setADSR( int aSamp, int dSamp, float sLvl, int rSamp );
    float env;
    int attackRate; //rate in samples
    int decayRate; //rate in samples
    float sustainLevel; //sustain value
    int sustainTime; //length in samples
    int releaseRate; //rate in samples   
    int state; 
  private:
    boolean autoNote;
    float attackStep;
    float decayStep;
    float releaseStep;
    int time; 
};

ADSR::ADSR() {
  state = 0;
  env = 0.0;
  setADSR( 4410, 4410, 0.8, 11050 );
  sustainTime = 0;
  autoNote = false;
}

void ADSR::setADSR( int aSamp, int dSamp, float sLvl, int rSamp ) {
  setAttack( aSamp );
  setDecay( dSamp );
  setSustain( sLvl );
  setRelease( rSamp );
}

void ADSR::setAttack( int aSamp ) {
  if( aSamp < 1 ){
    aSamp = 1;
  }
  attackRate = aSamp;
  attackStep = 1.0 / attackRate;
}

void ADSR::setDecay( int dSamp ) {
  if( dSamp < 1 ){
    dSamp = 1;
  }
  decayRate = dSamp;
  decayStep = -1.0 * ((1.0 - sustainLevel) / decayRate);
}
void ADSR::setSustain( float sLvl ) {
  sustainLevel = sLvl;
  setDecay( decayRate );
  setRelease( decayRate );
}
void ADSR::setSustainTime( int sSamp ) { 
  sustainTime = sSamp;
}
void ADSR::setRelease( int rSamp ) {
  if( rSamp < 1 ){
    rSamp = 1;
  }
  releaseRate = rSamp;
  releaseStep = -1.0 * (sustainLevel / releaseRate);
}

void ADSR::startEnv() {
  time = 0;
  //env = 0.0;
  state = 1;
}

void ADSR::releaseEnv() {
  state = 4;
}

void ADSR::triggerEnv(){
  autoNote = true;
  startEnv();
}
void ADSR::triggerEnv( int sSamp ){
  setSustainTime( sSamp );
  triggerEnv();
}

float ADSR::tick(){
  time++;
  switch( state ){
    case 0: // nothing
      break;
    case 1: //attack
      env += attackStep;
      if( env >= 1.0) {
        env = 1.0;
        state = 2;
      }
      break;
    case 2: //decay
      env += decayStep;
      if( env <= sustainLevel ){
        env = sustainLevel;
        state = 3;
      }
      break;
    case 3: //sustain
      if( autoNote && time > sustainTime + attackRate + decayRate ){
       state = 4; 
      }
      break;
    case 4: // release
      env += releaseStep;
      if( env <= 0.0 ){
        env = 0.0;
        state = 0;
        autoNote = false;
      }
  }
  return env;
}

#endif


