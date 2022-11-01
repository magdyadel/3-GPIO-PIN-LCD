/*
 * S2P_prog.c
 *
 *  Created on: Oct 29, 2022
 *      Author: Magdy Adel
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "util/delay.h"

#include "../../MCAL_AVR/DIO/DIO_interface.h"

#include "S2P_int.h"
#include "S2P_cfg.h"
#include "S2P_pvt.h"


void S2P_vInit(void)
{
	// init Shift pin , Serial data pin and Latch pin
	DIO_vWritePinDirection( S2P_PORT , S2P_SHIFT_CLOCK, DIO_OUTPUT);
	//DIO_vWritePinDirection( S2P_PORT , S2P_LATCH, DIO_OUTPUT);
	DIO_vWritePinDirection( S2P_PORT , S2P_DATA, DIO_OUTPUT);

	//DIO_vWritePinValue(S2P_PORT  ,  S2P_SHIFT_CLOCK  ,  DIO_LOW);

}

static void Shift_Data(void)//shift clk
{

	DIO_vWritePinValue(S2P_PORT  ,  S2P_SHIFT_CLOCK  ,  DIO_HIGH);
	_delay_us(1);//	1 micro sec
	DIO_vWritePinValue(S2P_PORT  ,  S2P_SHIFT_CLOCK  ,  DIO_LOW);
	_delay_us(1);//	1 micro sec

}

void S2P_vSendData(u8 A_u32Data)
{
	//Shift_Data();
	for(u8 i=0;i<=NO_REGS*8;i++)
	{
		DIO_vWritePinValue(S2P_PORT  ,  S2P_DATA  ,  GET_BIT(A_u32Data , 7-i));

		/* shift bit value into register */
		Shift_Data();

	}
//	Latch_Data();

}

