
#include <stdlib.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//             +-\/-+
//       PB5  1|    |8  Vcc
//       PB3  2|    |7  PB2 - ring detector
// led - PB4  3|    |6  PB1
//       GND  4|    |5  PB0 - buzzer
//             +----+

#define LED_PIN 4
#define led_init DDRB |= (1 << LED_PIN)
#define led_on PORTB |= (1 << LED_PIN)
#define led_off PORTB &= ~(1 << LED_PIN)
#define led_toogle PORTB ^= (1 << LED_PIN)

uint8_t ring_pulses;
uint8_t ringing;

/* Hardware Timer 0 (Jukebox Buzzer Ton Generator) */

#define N_LARGE 1024
#define N_LONG 512
#define N_DOUBLE 256
#define N_WHOLE 128
#define N_HALF 64
#define N_QUARTER 32
#define N_EIGHTH 16

#define N_C3 60 // 131Hz
#define N_D3 53 // 147Hz
#define N_E3 47 // 165Hz
#define N_F3 45 // 175Hz
#define N_G3 40 // 196Hz
#define N_A3 36 // 220Hz
#define N_B3 32 // 247Hz
#define N_C4 30 // 261Hz - do
#define N_D4 27 // 293Hz - re
#define N_E4 24 // 329Hz - mi
#define N_F4 22 // 349Hz - fa
#define N_G4 20 // 392Hz - sol
#define N_A4 18 // 440Hz - la
#define N_B4 16 // 493Hz - si
#define N_C5 15 // 522Hz
#define N_D5 13 // 586Hz
#define N_E5 12 // 658Hz
#define N_F5 11 // 698Hz
#define N_G5 10 // 784Hz
#define N_A5 9  // 880Hz
#define N_B5 8  // 986Hz

#define MUSIC_SIZE 24
#define MUSIC_TEMPO 2

const uint8_t music[][3] PROGMEM = {
    {N_G4, N_WHOLE, 0},
    {N_D4, N_WHOLE, 0},
    {N_E4, N_WHOLE, 0},
    {N_F4, N_WHOLE, N_HALF},

    {N_F4, N_HALF, 0},
    {N_F4, N_HALF, 0},

    {N_C4, N_WHOLE, 0},
    {N_D4, N_WHOLE, 0},
    {N_C4, N_WHOLE, 0},
    {N_D4, N_WHOLE, N_HALF},

    {N_D4, N_HALF, 0},
    {N_D4, N_HALF, 0},

    {N_C4, N_WHOLE, 0},
    {N_G4, N_WHOLE, 0},
    {N_F4, N_WHOLE, 0},
    {N_E4, N_WHOLE, N_HALF},

    {N_E4, N_HALF, 0},
    {N_E4, N_HALF, 0},

    {N_C4, N_WHOLE, 0},
    {N_D4, N_WHOLE, 0},
    {N_E4, N_WHOLE, 0},
    {N_F4, N_WHOLE, N_HALF},

    {N_F4, N_HALF, 0},
    {N_F4, N_HALF, 0}

};

#define TIMER0_NORMAL 0
#define TIMER0_PSCALER 3 // clk/64

#define buzzer_on TCCR0A |= (1 << COM0A0)
#define buzzer_off TCCR0A &= ~(1 << COM0A0)

void timer0_init(void)
{
    TCCR0A |= (1 << WGM01);
    TCCR0B = TIMER0_PSCALER;
    DDRB |= (1 << 0);
}

void note_delay(uint16_t note_value) {

    for(int i = 0; i < note_value; i++) {
        _delay_ms(MUSIC_TEMPO);
    }

}

void play_note(uint8_t note, uint8_t note_value, uint8_t pause)
{
    buzzer_on;
    OCR0A = note;
    note_delay(note_value);
    buzzer_off;
    note_delay(pause);
}

/* Hardware Timer 1 (Ring Detection Timing) */

#define TIMER1_PSCALER 14 // CK/8192 mode
#define TIMER1_1SEC_CONT 122

ISR(TIMER1_COMPA_vect)
{
    if (ringing > 0)
    {
        ringing--;
    }

    if (ring_pulses > 10)
    {
        ringing = 5;
    }

    ring_pulses = 0;
}

void timer1_init(void)
{
    TCCR1 |= (1 << CTC1) | TIMER1_PSCALER;
    OCR1C = TIMER1_1SEC_CONT;
    TIMSK |= (1 << OCIE1A);
    TCNT1 = 0;
}

/* Hardware External Interrupt 0 (Ring Detection Pulses) */

ISR(INT0_vect)
{
    if (ring_pulses == 0)
    {
        TCNT1 = 0;
    }
    ring_pulses++;
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
    led_init;
    int0_init();
    timer1_init();
    timer0_init();
    ring_pulses = 0;

    sei();

    for (;;)
    {

        for(int i = 0; i < MUSIC_SIZE; i++) {
            uint8_t note = pgm_read_byte(&(music[i][0]));
            uint8_t duration = pgm_read_byte(&(music[i][1]));
            uint8_t pause = pgm_read_byte(&(music[i][2]));
            play_note(note, duration, pause);
        }


        _delay_ms(2000);
        if (ringing > 0)
        {
            led_on;
        }
        else
        {
            led_off;
        }
    }

    return 0;
}