#include <mega8.h>
#include <delay.h>
// 1 Wire Bus functions
#asm
   .equ __w1_port=0x18 ;PORTB
   .equ __w1_bit=2
#endasm
#include <1wire.h>
#include <ds18b20.h>

#define digit1 PORTC.0
#define digit2 PORTC.1
#define digit3 PORTC.2
#define digit4 PORTC.3

flash char digits[] = {
0b11000000,                   //0
0b11111001,                   //1
0b10100100,                   //2
0b10110000,                   //3
0b10011001,                   //4  digits for anode
0b10010010,                   //5
0b10000010,                   //6
0b11111000,                   //7
0b10000000,                   //8
0b10010000,                   //9

0b11111111,                   //none 10
0b10111111                    //-    11
};


char digit_out[4], cur_dig ;
int indication;     

void recoding(void)
 {       
     if (indication<0)
     {
        if (indication>-100)
        {
          digit_out[0]=10;
          digit_out[1]=11;
          digit_out[3]=-indication%10;
          indication=indication/10;
          digit_out[2]=-indication%10;
        }
        else if (indication>-1000)
        {
          digit_out[0]=11;
          digit_out[3]=-indication%10;
          indication=indication/10;
          digit_out[2]=-indication%10;
          indication=indication/10;
          digit_out[1]=-indication%10;
        }
     }
     else
       if (indication<100)
       {
         digit_out[0]=10;
         digit_out[1]=10;
         digit_out[3]=indication%10;
         indication=indication/10;
         digit_out[2]=indication%10;
       } 
       else if (indication<1000)
       {
         digit_out[0]=10;
         digit_out[3]=indication%10;
         indication=indication/10;
         digit_out[2]=indication%10;
         indication=indication/10;
         digit_out[1]=indication%10;
       }
       else if (indication<10000)
       {
         digit_out[3]=indication%10;
         indication=indication/10;
         digit_out[2]=indication%10;
         indication=indication/10;
         digit_out[1]=indication%10;
         indication=indication/10;
         digit_out[0]=indication%10;
       }
     
 } 

// Timer 2 overflow interrupt service routine
interrupt [TIM2_OVF] void timer2_ovf_isr(void)
{
switch (cur_dig)
  {
  case 0:{digit4=0;digit1=1;PORTD=digits[digit_out[cur_dig]];break;}; 
  case 1:{digit1=0;digit2=1;PORTD=digits[digit_out[cur_dig]];break;};
  case 2:{digit2=0;digit3=1;PORTD=digits[digit_out[cur_dig]];PORTD.7=0;break;};
  case 3:{digit3=0;digit4=1;PORTD=digits[digit_out[cur_dig]];break;};
  }
  cur_dig++;   
  if (cur_dig==4) cur_dig=0;
}


void main(void)
{      
PORTB=0x00;
DDRB=0x00;

PORTC=0x00;
DDRC=0x0F;

PORTD=0x00;
DDRD=0xFF;

TCCR0=0x00;
TCNT0=0x00;

TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

ASSR=0x00;
TCCR2=0x04;
TCNT2=0x00;
OCR2=0x00;

MCUCR=0x00;

TIMSK=0x40;

ACSR=0x80;
SFIOR=0x00;
   
w1_init();
ds18b20_init(0,-20,110,DS18B20_12BIT_RES);

#asm("sei");

while(1)
      {
          indication=ds18b20_temperature(0)*10;  
          if (indication>20000)
          {
             indication=40960-indication;
             indication=-indication;
          }
          recoding();
          delay_ms(1000);
      }; 
}
