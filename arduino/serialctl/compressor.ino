#define COMPCTL ST34
#define COMPPIN 2
float psi;
void compressor_ctl(){
  static char pumping=0;
  int i;
  psi=0;
  for(i=0;i<5;i++){
    //5x multisampling
    psi += ADC2PSI(P_SENSOR);
  }
  psi = psi/5;
  if(comm_ok == 1){
    //Read pressures and pump if needed
    if(psi < MIN_PRESS){
      pumping = 1;
    } else if(psi >= MAX_PRESS){
      pumping = 0;
    }
  } else{
    //No control link, stop compressor
    pumping=0;
  }
  
  if(pumping == 1){
    //Go!
    COMPCTL.motor(COMPPIN,127);
  } else{
    //Stop!
    COMPCTL.motor(COMPPIN,0);
  }
}

