#include <xc.h>
#include <stdio.h>   // to use sprintf()
#include "ST7735.h"

#define CS LATBbits.LATB7  // SPI chip select pin
#define STRINGLENGTH 100


// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll, this uses external crystal*
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode*
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock,then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = ON // allow multiple reconfigurations
#pragma config IOL1WAY = ON // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

int main() {
    
  char str1[STRINGLENGTH],str2[STRINGLENGTH];
  int timer=10;
  float FPS=0.0;
  // some initialization function to set the right speed setting
  __builtin_disable_interrupts();
  
  // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;   // set LED an output pin
    LATAbits.LATA4 = 1;     // initialize LED high
    TRISBbits.TRISB4 = 1;   // set push button an input pin
  SPI1_init();
  LCD_init();
  LCD_clearScreen(BACKGROUND);

  __builtin_enable_interrupts();
  while(1) {
      for (timer=0;timer<=100;timer++){
            _CP0_SET_COUNT(0);
            sprintf(str1,"Hello World %d!  ",timer);
            LCD_drawString(28,32,str1,CYAN); //String
            LCD_drawBar(16,50,timer,RED);    //Bar
            FPS = 24000000/_CP0_GET_COUNT(); //FPS
            sprintf(str2,"FPS: %1.2f",FPS);
            LCD_drawString(28,80,str2,YELLOW);
            while(_CP0_GET_COUNT() < 2400000){;} // 24MHz/5Hz = 4800000
      }
      //LCD_clearScreen(BACKGROUND);
  }
  return 0;
}