/** \file usart.c
 *	\brief USART interface 
 *
 * \author Michał Drwięga
 * \date   2014.10.12
 * \version 1.0
 */ 

//_____  I N C L U D E S ___________________________________________________

#include "usart.h"

//_____ D E C L A R A T I O N S ____________________________________________

extern uint8_t txFlag;

//=============================================================================
void InitUsart()
{
	// Ustawienie parametrów transmisji szeregowej
	UBRRH = (uint8_t)(BAUD_PRESCALE >> 8);
	UBRRL = (uint8_t)BAUD_PRESCALE;       

	// Praca synchroniczna, brak kontroli parzystości, 1 bit stopu, 8 bitów danych
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);

	// Aktywne przerwanie od odbioru oraz zmiana trybu działania pinów D0 i D1
	UCSRB = (1<<RXEN)|(1<<TXEN) | (1 << RXCIE);	
}

//=============================================================================
void UsartTransmit1byte (uint8_t data)
{
	while (!(UCSRA & (1<<UDRE)));    // Waiting for free buffer
	UDR	= data;                     
}

//=============================================================================
void UsartTransmit1byteAndChecksum (uint8_t data)
{
	uint8_t checkSum;
	while (!(UCSRA & (1<<UDRE)));  // Oczekiwanie na pusty bufor.
	UDR	= data;                    // Wysłanie bajtu.

	checkSum = CheckSum8(data);
	while (!(UCSRA & (1<<UDRE)));  // Oczekiwanie na pusty bufor.
	UDR	= checkSum;                // Wysłanie sumy kontrolnej.
}

//=============================================================================
void UsartTransmit2byte (uint16_t data)
{
	while (!(UCSRA & (1<<UDRE)));    // Waiting for free buffer
	UDR	= (data >> 8);               // One byte send
	while (!(UCSRA & (1<<UDRE)));    
	UDR	= data;                      
}

//=============================================================================
void UsartTransmitData16(uint8_t order, uint16_t data)
{
	uint8_t checkSum;

	while (!(UCSRA & (1<<UDRE)));    // Oczekiwanie na pusty bufor.
	UDR	= order;                     // Wysłanie potwierdzenia rozkazu.

	checkSum = CheckSum8(order);

	while (!(UCSRA & (1<<UDRE)));	 // 1 bajt danych
	UDR	= (data >> 8);              

	while (!(UCSRA & (1<<UDRE)));    // 2 bajt danych
	UDR	= data;                      
		
	checkSum += CheckSum16(data);

	while (!(UCSRA & (1<<UDRE)));    
	UDR	= checkSum;                 
}

//=============================================================================
uint8_t USART_Receive (void)
{
	uint16_t cnt = 0;
	do { // Waiting for byte of data
		cnt++;
	} while (!(UCSRA & (1<<RXC)) && (cnt < UINT16_MAX));
	if (cnt >= UINT16_MAX)
		return 0;
	return UDR;              // Return received byte
}

//=============================================================================
ISR(USART_RXC_vect)
{
	uint8_t order, tmp, checksum;

	UCSRA |= (1 << RXC);

	order		= USART_Receive();
	checksum	= USART_Receive();	
	
	// Sprawdzanie sumy kontrolnej rozkazu i czy aktywowano komunikacje UART.
	if ((CheckSum8(order) == checksum))
	{	
		switch (order) // Wykonywanie określonego rozkazu.
		{
			case 0xC0: // Weryfikacja poprawności transmisji.			
				UsartTransmit1byte(0xC0);
				tmp = CheckSum8(0xC0);
				UsartTransmit1byte(0x55);
				tmp += CheckSum8(0x55);
				UsartTransmit1byte(tmp); // Wysłanie sumy kontrolnej.
				break;
			case 0xC1: // Wysłanie odczytów z różnych sensorów.
				if (txFlag == 0)
					txFlag = 1;
				break;			
			case 0xD0: // Polecenie uruchomienia oswietlenia quadrocoptera.
				UsartTransmit1byteAndChecksum(order);
				SET_ALL_LEDS;
				break;
			case 0xD1: // Polecenie wylaczenia oswietlenia quadrocoptera.
				UsartTransmit1byteAndChecksum(order);
				CLR_ALL_LEDS;
				break;
			default:
				break;
		}
	}
}