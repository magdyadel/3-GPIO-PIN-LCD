/*
 * lcd.c
 *
 *  Author: MAGDY
 */


#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
//#include <avr/io.h>
#include "../../MCAL_AVR/DIO/DIO_interface.h"
#include "../S2P/S2P_int.h"
//#include <util/delay.h>

#include "LCD_interface.h"
#include "LCD_private.h"
#include "LCD_config.h"


static u8 p=0,l=0;

void _delay_ms(u8 count){
	for(int i=0;i<count;i++);
}

static void LCD_KICK()
{
	DIO_vWritePinValue(LCD_CTRL, EN_SWITCH, DIO_HIGH );
	_delay_ms(2);
	DIO_vWritePinValue(LCD_CTRL, EN_SWITCH, DIO_LOW);

}

void LCD_clear_screen()
{
	 p=0;
	 l=0;
	LCD_WRITE_COMMAND(LCD_CMD_CLEAR_SCREEN);
}

void LCD_StepBack()
{
	p--;
	LCD_GOTO_XY(l,p);
}

void LCD_GOTO_XY(u8 line , u8 position)
{

	if(line==0)
	{
		if(position <16 && position >=0)
		LCD_WRITE_COMMAND(LCD_CMD_BEGIN_AT_FIRST_ROW + position);
	}
	else if (line==1)
	if(position <16 && position >=0)
	LCD_WRITE_COMMAND(LCD_CMD_BEGIN_AT_SECOND_ROW + position);
	l=0;p=0;
}


//initialize LCD
void LCD_Init(void)
{
	S2P_vInit();
	_delay_ms(20);

	DIO_vWritePinDirection(LCD_CTRL, EN_SWITCH, DIO_OUTPUT );

	DIO_vWritePinValue(LCD_CTRL, EN_SWITCH, DIO_LOW );
	_delay_ms(15);
	LCD_clear_screen();

	#ifdef EIGHT_BIT_MODE
//	DIO_vWritePortDirection(DataDir_LCD_PORT, 0xFF );
	//to do
	LCD_WRITE_COMMAND(LCD_CMD_FUNCTION_8BIT_2LINES);
	#endif

	#ifdef FOUR_BIT_MODE
	LCD_WRITE_COMMAND(0x02); // As datasheet
	LCD_WRITE_COMMAND(LCD_CMD_FUNCTION_4BIT_2LINES);
	#endif

	LCD_WRITE_COMMAND(LCD_CMD_ENTRY_MODE);
	LCD_WRITE_COMMAND(LCD_CMD_BEGIN_AT_FIRST_ROW);
	LCD_WRITE_COMMAND ( LCD_CMD_DISP_ON_CURSOR ) ;
	LCD_WRITE_COMMAND(LCD_CMD_DISP_ON_CURSOR_BLINK);
}


void LCD_WRITE_COMMAND(u8 command)
{
	#ifdef EIGHT_BIT_MODE
	LCD_isBUSY();
//	DIO_vWritePortValue(LCD_PORT,command);
//	DIO_vWritePinValue(LCD_CTRL ,RW_SWITCH , DIO_LOW );
//	DIO_vWritePinValue(LCD_CTRL ,RS_SWITCH , DIO_LOW );
	//To Do
	LCD_KICK();
	#endif

	#ifdef FOUR_BIT_MODE
	u8 cmd =0;
	cmd = (0x0F&(command >>4) ) ;//sending upper 4 bits 1100 1101
											// cmd =>	00001100
	S2P_vSendData(cmd);
	LCD_KICK();

	cmd=0;
	cmd = (0x0F & command ) ;//sending lower 4 bits 1100 1101
										// cmd =>	00001101
	S2P_vSendData( cmd );
	LCD_KICK();
	#endif
}


void LCD_WRITE_CHAR(u8 data)
{
	#ifdef EIGHT_BIT_MODE
	LCD_isBUSY();
	DIO_vWritePinValue(LCD_CTRL ,RS_SWITCH , DIO_HIGH );
	DIO_vWritePortValue(LCD_PORT,data);
	DIO_vWritePinValue(LCD_CTRL ,RS_SWITCH , DIO_HIGH );
	//DIO_vWritePinValue(LCD_CTRL ,RW_SWITCH , DIO_LOW );
	LCD_KICK();
	#endif

	#ifdef FOUR_BIT_MODE
	p++;
	if (p == 16) // Go to the second line
	{
		LCD_GOTO_XY(1,0); // Line 1 position 0
		l=0;
	}
	else if(p>16 && p < 32)
	{
		l=1;
	}
	else if ( p == 32 ) // Clear screen and show again
	{
		LCD_clear_screen();
		LCD_GOTO_XY(0,0); // Line 0 Position 0
		p = 0;
	}

	u8 d=0;
	d = ((data >> 4)&0x0F) ;//sending upper 4 bits
	SET_BIT(d,RS_SWITCH);
	S2P_vSendData(d);
	LCD_KICK();

	d=0;
	d =  (0x0F & data ) ;//sending lower 4 bits
	SET_BIT(d,RS_SWITCH);
	S2P_vSendData(d);
	LCD_KICK();

	#endif
}

void LCD_WRITE_STRING(u8 * str)
{
	int count = 0; // To count how much char on the line (it should be 16 char only)

	while (*str)
	{
		count++;
		LCD_WRITE_CHAR(*str++);
		if (count == 16) // Go to the second line
		{
			LCD_GOTO_XY(1,0); // Line 1 position 0
		}
		else if (count == 32 || count == 33) // Clear screen and show again
		{
			LCD_clear_screen();
			LCD_GOTO_XY(0,0); // Line 0 Position 0
			count = 0;
		}
	}

}

void LCD_WRITE_NUMBER(s32 tmpNUM)
{
	s32  num =tmpNUM;
	u8 digit=0;
	s32 rev=0;

	if(tmpNUM<0)
	{
		LCD_WRITE_CHAR( '-' );
		num *=-1;
	}
	if(tmpNUM%10==0)
		num+=1;

	while (num!=0)
	{
		digit = num%10;
		rev = rev*10+digit;
		num /= 10;
	}

	while (rev>0)
	{
		if((rev==1) && (tmpNUM%10==0) )
			rev=0;

		LCD_WRITE_CHAR( (rev%10) +'0' );
		rev /= 10;
	}

}




void LCD_save_CustomCHAR(u8 charArr[] ,u8 charID )
{
	LCD_WRITE_COMMAND( LCD_CMD_SET_CGRAM_ADDRESS + (8*charID) );
	for(int i=0;i<8;i++)
		LCD_WRITE_CHAR(charArr[i]);
	LCD_WRITE_COMMAND( LCD_CMD_SET_DDRAM_ADDRESS  );
}


void LCD_disp_CustomCHAR(u8 charID )
{

	LCD_WRITE_CHAR(charID);
}
