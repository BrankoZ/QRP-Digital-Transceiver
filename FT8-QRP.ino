// FT8QRP
// https://www.elektronik-labor.de/HF/FT8QRP.html
#include "si5351.h"
#include "Wire.h"

Si5351 si5351;
unsigned long  freq;

void setup(void)
{
  freq= 7074000;
  word cal_factor = 0;
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0); 
  si5351.set_correction(cal_factor, SI5351_PLL_INPUT_XO);
 
  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  si5351.set_freq(freq*100ULL, SI5351_CLK1);
  si5351.output_enable(SI5351_CLK1, 1);
 
  si5351.set_freq(freq*100ULL, SI5351_CLK0);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
  si5351.output_enable(SI5351_CLK0, 0);
  TCCR1A = 0x00;
  TCCR1B = 0x01; // Timer1 Timer 16 MHz
  TCCR1B = 0x81; // Timer1 Input Capture Noise Canceler
  ACSR |= (1<<ACIC);  // Analog Comparator Capture Input
  pinMode(7, INPUT); //PD7 = AN1 = HiZ, PD6 = AN0 = 0
  pinMode(13, OUTPUT);
}


void loop(void)
{
 // Modulationsfrequenz messen über Analog Comparator Pin7 = AN1

 unsigned int d1,d2;
 int FSK = 10;
 int FSKtx = 0;
 while (FSK>0){
  TCNT1 = 0;
  while (ACSR &(1<<ACO)){
    if (TCNT1>65000) {break;
  }
  }  while ((ACSR &(1<<ACO))==0){
    if (TCNT1>65000) {break;}
  }
  TCNT1 = 0;
  while (ACSR &(1<<ACO)){
    if (TCNT1>65000) {break;}
  }
  d1 = ICR1;  
  while ((ACSR &(1<<ACO))==0){
    if (TCNT1>65000) {break;}
  } 
  while (ACSR &(1<<ACO)){
    if (TCNT1>65000) {break;}
  }
  d2 = ICR1;
  if (TCNT1 < 65000){
  unsigned long codefreq = 1600000000/(d2-d1);
    if (codefreq < 350000){
      if (FSKtx == 0){
          digitalWrite(13,1);
          si5351.output_enable(SI5351_CLK1, 0);   //RX off
          si5351.output_enable(SI5351_CLK0, 1);   // TX on
      }
      si5351.set_freq((freq * 100 + codefreq), SI5351_CLK0);  
      FSKtx = 1;
    }
  }
  else{
    FSK--;
  }
 }
  digitalWrite(13,0);
  si5351.output_enable(SI5351_CLK0, 0);   //TX off
  si5351.output_enable(SI5351_CLK1, 1);   //RX on
  FSKtx = 0;
}
