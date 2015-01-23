/** \file config.h
 *	\brief Microcontroller configuration
 *
 * \author Michał Drwięga
 * \date   2014.10.12
 * \version 1.0
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU 8000000L //< Microcontroller frequency in Hz

//_____  I N C L U D E S ___________________________________________________

#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

//_____ D E F I N I T I O N S ______________________________________________

/*******************************************************************/
/* MAIN SETTINGS												   */
/*******************************************************************/

/// Selection of source of reference voltage for ADC -- AVCC
#define ADC_REF_CHANNEL			((0 << REFS1) | (1 << REFS0))

/// Macro for ADMUX settings
#define ADMUX_CONF(mux)		(mux | (0 << ADLAR) | ADC_REF_CHANNEL)

/// Reference voltage value
#define AREF_VALUE 5

/// Macro for scaling ADC measurement value in mV (VIN = ADC * VREF / 1024)
#define VIN(ADC)		(ADC * AREF_VALUE * 1000UL / 1024)		

/// Sonar prescaler value for calculating distance from time of flight
#define SONAR_PRESCALER 58

/// Pulse width which indicate that the new PPM frame arrived
#define MAX_PPM_WIDTH 3000

/*******************************************************************/
/* PORTS SETTINGS												   */
/*******************************************************************/
// Quadrocopter light settings
#define SET_LED1_OUT	DDRC  |= _BV(2)
#define SET_LED1		PORTC |= _BV(2)
#define CLR_LED1		PORTC &= ~_BV(2)

#define SET_LED2_OUT	DDRC  |= _BV(3)
#define SET_LED2		PORTC |= _BV(3)
#define CLR_LED2		PORTC &= ~_BV(3)

#define SET_LED3_OUT	DDRC  |= _BV(4)
#define SET_LED3		PORTC |= _BV(4)
#define CLR_LED3		PORTC &= ~_BV(4)

#define SET_LED4_OUT	DDRC  |= _BV(5)
#define SET_LED4		PORTC |= _BV(5)
#define CLR_LED4		PORTC &= ~_BV(5)

#define SET_LED5_OUT	DDRC  |= _BV(6)
#define SET_LED5		PORTC |= _BV(6)
#define CLR_LED5		PORTC &= ~_BV(6)

#define SET_LED6_OUT	DDRC  |= _BV(7)
#define SET_LED6		PORTC |= _BV(7)
#define CLR_LED6		PORTC &= ~_BV(7)

#define SET_ALL_LEDS	PORTC |= 0xFE
#define CLR_ALL_LEDS	PORTC &= ~0xFE

// Sonars ports settings
#define SET_SONAR1_EN_OUT   DDRD  |= _BV(4)
#define SET_SONAR1_EN       PORTD &= ~_BV(4)
#define CLR_SONAR1_EN       PORTD |= _BV(4)

#define SET_SONAR2_EN_OUT   DDRD  |= _BV(5)
#define SET_SONAR2_EN       PORTD &= ~_BV(5)
#define CLR_SONAR2_EN       PORTD |= _BV(5)

#define SET_SONAR1_IN		DDRD  &= ~_BV(2)
#define SET_SONAR2_IN		DDRD  &= ~_BV(3)

// PPM signal input port
#define SET_PPM_IN			DDRB  &= ~_BV(2)

// FailSafe 
#define SET_FAILSAFE_OUT	DDRB  |= _BV(1)
#define SET_FAILSAFE		PORTB |= _BV(1)
#define CLR_FAILSAFE		PORTB &= ~_BV(1)

//_____ D E C L A R A T I O N S ____________________________________________

/** \brief Microcontroller ports initializatin
 */
void InitPorts();

/** \brief Implements delay in ms
 */
void wait_ms(int ms);

/** \brief Implements delay in us
 */
void wait_us(int us);

/** \brief Calculate checksum for 1-byte variable
*/
uint8_t CheckSum8(uint8_t data);

/** \brief Calculate checksum for 2-byte variable
 */
uint8_t CheckSum16(uint16_t data);

#endif /* CONFIG_H_ */