/** \file usart.h
 *	\brief Obsługa interfejsu USART
 *		   Zaprojektowany protokół komunikacji.
 *
 * \author Michal Drwiega
 * \date   2014.10.12 
 * \version 1.0
 *
 *
 * W pierwszym odebranym bajcie danych dwa najstarsze bity są bitami
 * kontrolnymi o wartościach wysokiego stanu logicznego (11).
 * Kolejne 6 bitów określa rozkaz, który należy wykonać.
 * W ten sposób można zakodować do 64 różnych rozkazów, co wydaje
 * się byc wartoscią zupełnie wystarczającą. Zaimplementowany protokól zakłada
 * przesyłanie bardziej znaczących bajtów przed mniej znaczacymi (Big endian).
 * Rozkazy podzielono na cztery grupy w zależności od wartści pierwszych
 * czterech bitów:
 *		- 0xC (1100) - żądania przesłania informacji zwrotnej.
 *		- 0xD (1101) - rozkazy konfiguracyjne.
*		- 0xE (1110) - pozostałe rozkazy.
*		- 0xF (1111) - pozostałe rozkazy.
*
* Poniżej znajduje się lista rozkazów z opisem.
*
* 1100 0000 - 0xC0
*		Wstepna weryfikacja poprawności działania transmisji. 
* 		Oczekuje się odpowiedzi w postaci 0101 0101 - 0x55.
*
* 1100 1001 - 0xC1
*		Polecenie wysłania odczytanych wartości z sensorow odległości.
*		Kolejność wysyłanych pomiarów jest następująca:
*			- 2 bajty - napięcie w mV odczytane z sensora Sharp 1,
*			- 2 bajty - napięcie w mV odczytane z sensora Sharp 2,
*			- 2 bajty - napięcie w mV odczytane z sensora Sharp 3,
*			- 2 bajty - napięcie w mV odczytane z sensora Sharp 4,
*			- 2 bajty - napięcie w mV odczytane z sensora Sharp 5,
*			- 2 bajty - napięcie w mV odczytane z sensora Sharp 6,
*			- 2 bajty - napięcie w mV odczytane z sensora Sharp 7,
*			- 2 bajty - zmierzona odleglosc za pomoca sonaru nr 1,
*			- 2 bajty - zmierzona odleglosc za pomoca sonaru nr 2,
*			- 16 bajtow - wartosci zmierzone dla poszczegolnych kanalow PPM,
* 
*
* 1110 0000 - 0xD0
*		Polecenie uruchomienia oswietlenia quadrocoptera.
* 1110 0000 - 0xD1
*		Polecenie wylaczenia oswietlenia quadrocoptera.
*
*/ 

#ifndef USART_H_
#define USART_H_

//_____  I N C L U D E S ___________________________________________________
#include "config.h"
#include "sensors_interface.h"

//_____ D E F I N I T I O N S ______________________________________________

/// Baudrate speed value
#define USART_BAUDRATE 9600

/// Calculation of baudrate speed register value
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

//_____ D E C L A R A T I O N S ____________________________________________

/* \brief USART interface initialization
 */
void InitUsart();

/** \brief Send one byte of data by USART interface
 */
void UsartTransmit1byte (uint8_t data);

/** \brief Send 2-byte variable by usart interface.
 *		   Use Big Endian convention, czyli bardziej znaczący
 *		   bajt jako pierwszy.
 */
void UsartTransmit2byte (uint16_t data);

/** \brief USART data receive
 */ 
uint8_t USART_Receive (void);

/** \brief Interrupt from received data by USART
 */
ISR(USART_RXC_vect);

#endif /* USART_H_ */