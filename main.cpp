
#include <stdlib.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define OC1A_PIN

//       +-\/-+
// PB5  1|    |8  Vcc
// PB3  2|    |7  PB2
// PB4  3|    |6  PB1
// GND  4|    |5  PB0 - OC1A
//       +----+

int main(void)
{

    DDRB |= (1 << 4);

    for (;;)
    {
        _delay_ms(100);
        PORTB ^= (1 << 4);
    }

    return 0;
}