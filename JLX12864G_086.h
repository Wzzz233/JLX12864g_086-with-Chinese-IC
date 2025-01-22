#ifndef __JLX12864G_086_H_
#define __JLX12864G_086_H_

#include "msp430g2553.h"

// ¶¨ÒåÒý½Å
#define ROM_IN 1    // P1.1
#define ROM_OUT 2   // P1.2
#define ROM_SCK 3   // P1.3
#define ROM_CS 5    // P1.5
#define SCLK 2      // P2.2
#define SDA 3       // P2.3
#define RS 4        // P2.4
#define RESET 1     // P2.1
#define CS 5        // P2.5

#define set(_register,_bit) _register|=(0x01<<_bit)
#define unset(_register,_bit) _register&=~(0x01<<_bit)

void setup();
void JLX_Write_Command(int data1);
void JLX_Write_Data(int data1);
void JLX_init();
void JLX_address(unsigned char page, unsigned char column);
void JLX_clearscreen();
void Display(unsigned char *dp);
void Display_Word_16x16(unsigned char page, unsigned char column, unsigned char *dp);
void Display_Word_8x16(unsigned char page, unsigned char column, unsigned char *dp);
void send_command_to_Rom(unsigned char data);
static unsigned char get_data_from_ROM();
void get_and_write_16x16(unsigned long fontaddr, unsigned char page, unsigned char column);
void get_and_write_8x16(unsigned long fontaddr, unsigned char page, unsigned char column);
void display_string(unsigned char page, unsigned char column, unsigned char *text);


#endif
