/** \file config.c
 *	\brief Microcontroller configuration
 *
 * \author Michał Drwięga
 * \date   2014.11.28
 * \version 1.1
 */

#include "config.h" 

//=============================================================================
void InitPorts()
{
	SET_LED1_OUT;	SET_LED2_OUT;
	SET_LED3_OUT;	SET_LED4_OUT;
	SET_LED5_OUT;	SET_LED6_OUT;
	SET_SONAR1_EN_OUT;	SET_SONAR2_EN_OUT;
	SET_SONAR1_IN; SET_SONAR2_IN;
	SET_PPM_IN;
	SET_FAILSAFE_OUT;
}

//=============================================================================
void wait_ms(int ms)
{
	for(int i=0; i < ms ;i++)
	{
		_delay_ms(1);		
	}
}

//=============================================================================
void wait_us(int us)
{
	for(int i=0; i < us ;i++)
	{
		_delay_us(1);		
	}
}

//=============================================================================
uint8_t CheckSum8(uint8_t data)
{
	uint8_t sum = 0;
	
	while (data > 0)
	{
		sum += data & 1;
		data >>= 1;
	}
	return sum;
}

//=============================================================================
uint8_t CheckSum16(uint16_t data)
{
	uint8_t sum = 0;
	
	while (data > 0)
	{
		sum += data & 1;
		data >>= 1;
	}
	return sum;
}