
#include <stdlib.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//       +-\/-+
// PB5  1|    |8  Vcc
// PB3  2|    |7  PB2 - ring detector
// PB4  3|    |6  PB1
// GND  4|    |5  PB0
//       +----+

/* Hardware Timer 0 */

#define TIMER0_NORMAL 0
#define TIMER0_PSCALER 5

void timer0_init(void)
{
    TCCR0A = 0;
    TCCR0B = TIMER0_1024_PSCALER;
}

/* Hardware Timer 1 */

#define TIMER1_PSCALER 14 // CK/8192 mode
#define TIMER1_1SEC_CONT 122

ISR(TIMER1_COMPA_vect)
{
    PORTB ^= (1 << 4);
}

void timer1_init(void)
{
    TCCR1 |= (1 << CTC1) | TIMER1_PSCALER;
    OCR1C = TIMER1_1SEC_CONT;
    TIMSK |= (1 << OCIE1A);
}

/* Hardware External Interrupt 0 */

ISR(INT0_vect)
{
    PORTB ^= (1 << 4);
}

void int0_init(void)
{
    MCUCR |= (1 << ISC01);
    GIMSK |= (1 << INT0);
    DDRB &= ~(1 << 2);
    PORTB |= (1 << 2);
}

/* Main */

int main(void)
{

    DDRB |= (1 << 4);
    int0_init();
    timer1_init();

    sei();

    for (;;)
    {
        _delay_ms(100);
        // PORTB ^= (1 << 4);
    }

    return 0;
}