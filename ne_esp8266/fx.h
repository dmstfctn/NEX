uint16_t fx_hardclip( uint16_t sample, int amount ){
  amount = amount + 1;
  return constrain( sample * amount, 0, 65534 );
}

