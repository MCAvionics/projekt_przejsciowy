/** \file sensors_interface.c
*	\brief Main file of sensors interface program
 *
 * \author Michal Drwiega
 * \date   2014.11.10
 * \version 1.2
 */ 

#include "sensors_interface.h"
       
/// Voltages on SHARP sensors
volatile uint16_t  sharpVoltage[7];

/// Distances measured by sonars
volatile uint16_t  sonarValue[2];

/// Flag indicate if send measurements is allowed
volatile uint8_t txFlag = 0;

/// Counter for calculating measure intervals
volatile uint8_t cnt = 0;

/// Counter for calcultaing PPM pulses width
volatile uint16_t timeCntPPM = 0;

/// Indicates which channel of PPM signal is measured
volatile uint8_t channelPPMcnt = 0;

/// Store measured values for PPM channels
volatile uint16_t channelPPM[8];

volatile uint8_t newFrame = 0;

//=============================================================================
ISR (TIMER0_COMP_vect) // Frequency 100Hz
{
	// Measure voltages on SHARP sensors
	for (uint8_t i=0; i<7; i++)
	{
		sharpVoltage[i] = VIN(MeasureADC(ADMUX_CONF((i+1))));
	}	
	if (++cnt > 40)
		cnt = 0;		
	if (cnt == 10)
		StartMeasureSonarPW(1);		
	else if (cnt == 30)	
		StartMeasureSonarPW(2);
	
	TIFR |= 0x03;  // Clear flag from timer0.
}

//=============================================================================
ISR(TIMER1_OVF_vect) // Reset sonars measurements
{
	cli();
	TCNT1 = 0; // Timer1 reset
	GICR &= ~((1 << INT0) | (1 << INT1));
	GIFR |= (1 << INTF0) | (1 << INTF1);	
	sei();
	TIFR |= (1 << TOV1);  // Clear flag from timer1.
}

//=============================================================================
ISR(TIMER2_OVF_vect) // Increment time counter value
{
	timeCntPPM++;
}

//=============================================================================
ISR (INT0_vect)
{
	if (MCUCR & _BV(ISC00)) // Rising edge interrupt
	{
		cli();
		TCNT1 = 0; // Timer1 reset
		sei();
		MCUCR &= ~_BV(ISC00); // Change to falling edge detect on INT0
	}
	else // Falling edge interrupt
	{
		cli();
		sonarValue[0] = TCNT1 / SONAR_PRESCALER;
		sei();
		
		GICR &= ~(1 << INT0);
	}		
	GIFR |= (1 << INTF0);	// Interrupt flag clear
}

//=============================================================================
ISR (INT1_vect)
{
	cli();
	if (MCUCR & _BV(ISC10)) // Rising edge interrupt
	{
		TCNT1 = 0; // Timer1 reset
		MCUCR &= ~_BV(ISC10); // Change to falling edge detect on INT0
	}
	else // Falling edge interrupt
	{
		sonarValue[1] = TCNT1 / SONAR_PRESCALER;
		
		GICR &= ~(1 << INT1);
	}
	GIFR |= (1 << INTF1);	// Interrupt flag clear
	sei();
}

//=============================================================================
ISR (INT2_vect) // PPM
{
	cli();

	if ((timeCntPPM + TCNT2) > MAX_PPM_WIDTH) // new frame
	{
		newFrame = 1;
		channelPPMcnt = 0;
	}
	else if(newFrame) // measurement for properly PPM channel
	{
		channelPPM[channelPPMcnt] = timeCntPPM + TCNT2;
		if (channelPPMcnt < 7) // increase channel counter
		{
			channelPPMcnt++;
		}
		else
		{
			newFrame = 0;
		}
	}
	TCNT2 = 0; // Timer2 reset
	timeCntPPM = 0;
	
	GIFR |= (1 << INTF1);	// Interrupt flag clear
	sei();
}

//=============================================================================
void StartMeasureSonarPW(int nr)
{
	// Check if previous measurements ends
	if ((~GICR & _BV(INT0)) && (~GICR & _BV(INT1)))
	{
		if (nr == 1)
		{
			MCUCR |= ((1 << ISC01) | (1 << ISC00)); // Rising edge on INT0
			GICR |= (1 << INT0);					// Enable INT0 interrupts
		}
		else if (nr == 2)
		{
			MCUCR |= ((1 << ISC11) | (1 << ISC10)); // Rising edge on INT1
			GICR |= (1 << INT1);					// Enable INT1 interrupts
		}
	}
}

//=============================================================================
void InitTimer0()
{
	TCCR0 = (1 << WGM01) | (0 << WGM00) |		// CTC Mode,
	(0 << COM01) | (0 << COM00) |				// OC0 disconnected,
	(1 << CS02) | (0 << CS01) | (1 << CS00);	// prescaler  = 64.
	OCR0 = 77; 	       // f = f_clk/(2N*(1+OCR0)) -> f = 100Hz)
	
	TIMSK |= (1 << OCIE0); // Output Compare Match Interrupt Enable.
}

//=============================================================================
void InitTimer1()
{
	// Normal mode, TOP=0xFFFF, prescaler=8 => 1us resolution 
	TCCR1A = 0;
	TCCR1B = (0 << CS12) | (1 << CS11) | (0 << CS10);
	TIMSK |= (1 << TOIE1);
	TCNT1 = 0;
}

//=============================================================================
void InitTimer2()
{
	// Normal mode, TOP=0xFF, prescaler=8 => 1us resolution
	TCCR2 = (0 << CS22) | (1 << CS21) | (0 << CS20); // Freq = 1MHz, 
	TIMSK |= (1 << TOIE2);
	TCNT2 = 0;
}

//=============================================================================
void InitADC()
{
	ADCSRA = (1 << ADEN) | (0 << ADSC) | (0 << ADIE) |
	(1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0); // ADC prescaler = 64.
	
	ADMUX = ADMUX_CONF(0); // First conversion - 25 cycles.
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC))) { }
}


//=============================================================================
uint16_t MeasureADC(uint8_t admuxSetting)
{
	ADMUX = admuxSetting;
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC))) { }
	return (ADCL + (ADCH << 8));	
}

//=============================================================================
void UsartTransmitData()
{
	uint8_t checksum = CheckSum8(0xC1);

	while (!(UCSRA & (1<<UDRE)));    // Waiting for free buffer
	UDR	= 0xC1;                      // Send confirmation of order
	
	cli();
	// Transmit SHARPs measurements
	for (uint8_t i=0; i<7; i++)
	{
		UsartTransmit2byte(sharpVoltage[i]);
		checksum += CheckSum16(sharpVoltage[i]);
	}
	// Transmit sonars measurements
	UsartTransmit2byte(sonarValue[0]);
	checksum += CheckSum16(sonarValue[0]);
	UsartTransmit2byte(sonarValue[1]);
	checksum += CheckSum16(sonarValue[1]);
	// Transmit PPM channels data
	for (uint8_t i=0; i<7; i++)
	{
		UsartTransmit2byte(channelPPM[i]);
		checksum += CheckSum16(channelPPM[i]);
	}
	sei();
	
	while (!(UCSRA & (1<<UDRE)));
	UDR	= checksum;
}

//=============================================================================
int main(void)
{	
	InitPorts();  // Ports initialization
	InitADC();	  // ADC initialization
	InitTimer0(); // Initialization Timer 0, which generate interrupts for measurements
	InitTimer1(); // Initialization Timer 1
	InitTimer2(); // Initialization Timer 2, which measure pulses for decoding PPM
	InitUsart();  // USART initialization	

	SET_SONAR1_EN;
	SET_SONAR2_EN;
	
	// Enable IN2 interrupts for detecting PPM signal
	MCUCR |= (1 << ISC2);	// Rising edge detect on INT2
	GICR |= (1 << INT2);

	sei(); // Turn on global interrupts

	while (1)
	{
		if (txFlag) // Send data by USART
		{
			UsartTransmitData();
			txFlag = 0;
		}			
	}
}