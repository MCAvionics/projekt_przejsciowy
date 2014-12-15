/** \file sensors_interface.h
*	\brief Main file of sensors interface program
 *
 * \author Michal Drwiega
 * \date   2014.11.10
 * \version 1.2
 */ 

#ifndef SENSORS_INTERFACE_H_
#define SENSORS_INTERFACE_H_

//_____  I N C L U D E S ___________________________________________________

#include "config.h"
#include "usart.h"

//_____ D E C L A R A T I O N S ____________________________________________

/** \brief Measurements interrupt
 */
ISR (TIMER0_COMP_vect);
ISR(TIMER1_OVF_vect);
ISR(TIMER2_OVF_vect);
ISR (INT0_vect);
ISR (INT1_vect);
ISR (INT2_vect);

/** \brief Timer0 initialization for generate interrupts for synchronous measurements
 */
void InitTimer0();

/** \brief Timer1 initialization to time measurement
 */
void InitTimer1();

/** \brief Timer2 initialization to time measurement for PPM decoder
 */
void InitTimer2();

/** \brief External interrupts initialization for sonars measurements
 */
void StartMeasureSonarPW(int nr);

/** \brief ADC initialization
 */
void InitADC();

/** \brief Make single measurement of voltage on selected ADC channel
 *
 * \param admuxSetting - ADMUX register setting for current measurement
 *
 * \return Return measured value of voltage
 */
uint16_t MeasureADC(uint8_t admuxSetting);

void UsartTransmitData();

#endif  /* SENSORS_INTERFACE_H_ */