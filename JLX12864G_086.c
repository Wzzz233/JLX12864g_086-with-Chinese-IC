#include "JLX12864G_086.h"
#include "delay.h"

void setup()
{
    unset(P2SEL,SCLK);           // enable
    set(P2DIR,SCLK);             // output
    unset(P2SEL,SDA);           // enable
    set(P2DIR,SDA);             // output
    unset(P2SEL,RS);           // enable
    set(P2DIR,RS);             // output
    unset(P2SEL,RESET);           // enable
    set(P2DIR,RESET);             // output
    unset(P2SEL,CS);           // enable
    set(P2DIR,CS);             // output

    unset(P1SEL,ROM_IN);           // enable
    set(P1DIR,ROM_IN);             // output
    unset(P1SEL,ROM_OUT);           // enable
    unset(P1DIR,ROM_OUT);             // input
    unset(P1SEL,ROM_SCK);           // enable
    set(P1DIR,ROM_SCK);             // output
    unset(P1SEL,ROM_CS);           // enable
    set(P1DIR,ROM_CS);             // output
    P2OUT = 0x00;                 // unset all bits in P1OUT
    P1OUT = 0x00;
}


//写指令到LCD模块
void JLX_Write_Command(int data1){
    char i;
    unset(P2OUT, CS);
    unset(P2OUT, RS);
    for(i=0; i<8; i++){
        unset(P2OUT, SCLK);
        Delay_us(10);
        if(data1 & 0x80)
            set(P2OUT, SDA);
        else
            unset(P2OUT, SDA);
        set(P2OUT, SCLK);
        Delay_us(10);
        data1=data1<<=1;
    }
}

//写数据
void JLX_Write_Data(int data1){
    unset(P2OUT, CS);
    set(P2OUT, RS);
    char i;
    for(i=0; i<8; i++)
    {
        unset(P2OUT, SCLK);
        if(data1&0x80)
            set(P2OUT, SDA);
        else
            unset(P2OUT, SDA);
        set(P2OUT, SCLK);
        data1=data1<<=1;
    }
    set(P2OUT, CS);
}

//初始化
void JLX_init(){
    unset(P2OUT,RESET);
    Delay_ms(100);
    set(P2OUT,RESET);
    Delay_ms(100);
    JLX_Write_Command(0xe2);
    Delay_ms(5);
    JLX_Write_Command(0x2c);
    Delay_ms(50);
    JLX_Write_Command(0x2e);
    Delay_ms(50);
    JLX_Write_Command(0x2f);
    Delay_ms(5);
    JLX_Write_Command(0x23);
    JLX_Write_Command(0x81);
    JLX_Write_Command(0x28);
    JLX_Write_Command(0xa2);
    JLX_Write_Command(0xc8);
    JLX_Write_Command(0xa0);
    JLX_Write_Command(0x40);
    JLX_Write_Command(0xaf);
}

void JLX_address(unsigned char page, unsigned char column){
    column = column - 0x01;
    JLX_Write_Command(0xb0 + page - 1);
    JLX_Write_Command(((column>>4)&0x0f)+0x10);
    JLX_Write_Command(column&0x0f);
}

void JLX_clearscreen()
{
    unsigned char i,j;

    for(i = 0; i < 9; i++)
    {
        JLX_Write_Command(0xb0 + i);
        JLX_Write_Command(0x10);
        JLX_Write_Command(0x00);
        for(j = 0; j < 132; j++)
            JLX_Write_Data(0x00);
    }
}

void Display(unsigned char *dp)
{
    unsigned int i, j;
    for(j=0; j<8; j++)
    {
        JLX_address(j+1, 1);
        for (i=0;i<128;i++)
        {
            JLX_Write_Data(*dp);
            dp++;
        }
    }
}

void Display_Word_16x16(unsigned char page, unsigned char column, unsigned char *dp)
{
    unsigned int i, j;
    for(j=0;j<2;j++)
    {
        JLX_address(page+j,column);
        for (i=0;i<16;i++)
        {
            JLX_Write_Data(*dp);
            dp++;
        }
    }
}

void Display_Word_8x16(unsigned char page, unsigned char column, unsigned char *dp)
{
    unsigned int i, j;
    for(j=0;j<2;j++)
    {
        JLX_address(page+j,column);
        for (i=0;i<8;i++)
        {
            JLX_Write_Data(*dp);
            dp++;
        }
    }
}

void send_command_to_Rom(unsigned char data)
{
    unsigned char i;
    for(i=0;i<8;i++ )
    {
        unset(P1OUT, ROM_SCK);
        Delay_us(10);
        if(data&0x80)
            set(P1OUT, ROM_IN);
        else
            unset(P1OUT, ROM_IN);
        data = data << 1;
        set(P1OUT, ROM_SCK);
        Delay_us(10);
    }
}

static unsigned char get_data_from_ROM()
{
    unsigned char i;
    unsigned char ret_data = 0;
    for(i=0;i<8;i++)
    {
        set(P1OUT, ROM_OUT);
        unset(P1OUT, ROM_SCK);
        Delay_us(1);
        ret_data = ret_data << 1;
        if (P1IN & (1 << ROM_OUT))
            ret_data = ret_data + 1;
        else
            ret_data = ret_data + 0;
        set(P1OUT, ROM_SCK);
        Delay_us(1);
    }
    return(ret_data);
}

void get_and_write_16x16(unsigned long fontaddr, unsigned char page, unsigned char column)
{
    unsigned char i,j,disp_data;
    unset(P1OUT, ROM_CS);
    send_command_to_Rom(0x03);
    send_command_to_Rom((fontaddr&0xff0000)>>16);
    send_command_to_Rom((fontaddr&0xff00)>>8);
    send_command_to_Rom(fontaddr&0xff);
    for(j=0;j<2;j++)
    {
        JLX_address(page+j,column);
        for(i=0; i<16; i++ )
        {
            disp_data = get_data_from_ROM();
            JLX_Write_Data(disp_data);
        }
    }
    set(P1OUT, ROM_CS);
}

void get_and_write_8x16(unsigned long fontaddr, unsigned char page, unsigned char column)
{
    unsigned char i,j,disp_data;
    unset(P1OUT, ROM_CS);
    send_command_to_Rom(0x03);
    send_command_to_Rom((fontaddr&0xff0000)>>16);
    send_command_to_Rom((fontaddr&0xff00)>>8);
    send_command_to_Rom(fontaddr&0xff);
    for(j=0;j<2;j++)
    {
        JLX_address(page+j,column);
        for(i=0; i<8; i++ )
        {
            disp_data = get_data_from_ROM();
            JLX_Write_Data(disp_data);
        }
    }
    set(P1OUT, ROM_CS);
}

unsigned long fontaddr = 0;
void display_string(unsigned char page, unsigned char column, unsigned char *text)
{
    unsigned char i = 0;
    while(text[i] > 0x00)
    {
        if((text[i] >= 0xb0) && (text[i] <= 0xf7) && (text[i + 1] >= 0xa1))
        {
            //国标简体（GB2312）汉字在晶联讯字库 IC 中的地址由以下公式来计算：
            //Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
            //由于担心 8 位单片机有乘法溢出问题，所以分三部取地址
            fontaddr = (text[i]- 0xb0)*94;
            fontaddr += (text[i+1]-0xa1)+846;
            fontaddr = (unsigned long)(fontaddr*32);

            get_and_write_16x16(fontaddr,page,column);
            i += 2;
            column += 16;
        }
        else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
        {
            //国标简体（GB2312）15x16 点的字符在晶联讯字库 IC 中的地址由以下公式来计算：
            //Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
            //由于担心 8 位单片机有乘法溢出问题，所以分三部取地址
            fontaddr = (text[i]- 0xa1)*94;
            fontaddr += (text[i+1]-0xa1);
            fontaddr = (unsigned long)(fontaddr*32);
            get_and_write_16x16(fontaddr,page,column); //从指定地址读出数据写到液晶屏指定（page,column)座标中
            i+=2;
            column+=16;
        }
        else if((text[i]>=0x20) &&(text[i]<=0x7e))
        {
            fontaddr = (text[i]- 0x20);
            fontaddr = (unsigned long)(fontaddr*16);
            fontaddr = (unsigned long)(fontaddr+0x3cf80);
            get_and_write_8x16(fontaddr,page,column); //从指定地址读出数据写到液晶屏指定（page,column)座标中
            i+=1;
            column+=8;
        }
        else i++;
    }
}
