#include <xc.h>
#include <stdio.h>
#include "i2c_master_noint.h"
#include "ST7735.h"
#include "LSM6DS333.h"

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

unsigned char imu_test(void) {
  // get the data from WHO_AM_I register
  unsigned char whoami;
  i2c_master_start();                   // start
  i2c_master_send(IMU_ADDR << 1);   // OP + W: R/W = 1 write
  i2c_master_send(0x0F);                // ADDR: WHO_AM_I register for imu

  i2c_master_restart();                 // restart
  i2c_master_send(IMU_ADDR << 1 | 1);   // OP + W: R/W = 1 read
  whoami = i2c_master_recv();           // CTRL1_XL register: defalut [0 1 1 0 1 0 0 1]
  i2c_master_ack(1);
  i2c_master_stop();
  return whoami;
}



int main() {
    unsigned char dataReg8[STRINGLENGTH]={};
    // short dataReg16[STRINGLENGTH];
    char str1[STRINGLENGTH];
    int i;
    float Gx,Gy;
    
    __builtin_disable_interrupts();
    
    
    TRISAbits.TRISA4 = 0; // set LED an output pin
    TRISBbits.TRISB4 = 1; // set push button an input pin
    LATAbits.LATA4 = 1; // turn LED off
    LSM6DS333_init();
    LCD_init();
    LCD_clearScreen(BACKGROUND);

    __builtin_enable_interrupts();

    while(1) {
        _CP0_SET_COUNT(0);
        I2C_read_multiple(IMU_ADDR, 0x20, dataReg8, 14);
        Gx = getxXL(dataReg8);
        Gy = getyXL(dataReg8);    
        sprintf(str1,"Gx = %1.3f!",Gx);
        LCD_drawString(10,10,str1,BLUE); //String
        sprintf(str1,"Gy = %1.3f!",Gy);
        LCD_drawString(10,20,str1,BLUE); //String
        LCD_drawGravCross(Gx,Gy,WHITE);
        
        
//       unsigned char status = imu_test();
//       sprintf(str1, "Test address: %x  ", status);
//       LCD_drawString(10, 65, str1, WHITE);
//        

        
        while (_CP0_GET_COUNT()<1200000) {;}
        LATAbits.LATA4=!LATAbits.LATA4;
    }
    return 0;
}
