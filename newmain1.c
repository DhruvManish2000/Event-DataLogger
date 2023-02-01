/* 
 * File:   newmain.c
 * Author: dhruvmanish --- CCID: 1628666
 * 
 * Project Description: 
 * Event (Serial) Logger - that has 4 channels, logs input when the button is pressed
 * Displays the time the button was pressed with respect to the time the code was run
 *
 * Created on November 13, 2021, 7:08 PM
 */

#define F_CPU 1000000UL 
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// variables for the time initialized to 0
uint8_t tenth = 0;
uint8_t seconds = 0;
uint8_t minutes = 0;
uint8_t hours = 0;
uint16_t days = 0;

// Character arrays for the timer values to be sent to the Tera Term
char tenth_ [] = "00";
char seconds_ [] = "00";
char minutes_ [] = "00";
char hours_ [] = "00";
char days_ [] = "000";


/*
 * 
 */

// Function initialization
void IO_in(void);
void sending_info(unsigned char data);
void Interrupts(void);
void clock(void);
void converting_time(void);
void sending_time(void);
void sending_nameid(void);
void buttons_config(void);

ISR(TIMER0_COMPA_vect, ISR_BLOCK) {
    tenth++;
    if (tenth == 10) {
        tenth = 0;
        seconds++;
    }
    if (seconds == 60) {
        seconds = 0;
        minutes++;
    }
    if (minutes == 60) {
        minutes = 0;
        hours++;
    }
    if (hours == 24) {
        hours = 0;
        days++;
    }
}

int main(int argc, char** argv) {

    // All the functions are called here
    IO_in();
    clock();
    Interrupts();
    sending_nameid();
    buttons_config();

    return (EXIT_SUCCESS);
}

void clock(void) {
    TCCR0A |= (1 << WGM01);
    TIMSK |= (1 << OCIE0A);
    OCR0A = 179;
    TCCR0B |= (1 << CS02) | (1 << CS00);
}

void IO_in(void) {
    // Make the ports input and turn on pull ups
    DDRD &= ~(1 << DDD3); // preparing PD3 is input
    DDRD &= ~(1 << DDD4); // preparing PD4 is input
    DDRD &= ~(1 << DDD5); // preparing PD5 is input
    DDRD &= ~(1 << DDD6); // preparing PD6 is input
    PORTD |= (1 << PORTD3); // Turn on pull ups 
    PORTD |= (1 << PORTD4); // Turn on pull ups 
    PORTD |= (1 << PORTD5); // Turn on pull ups
    PORTD |= (1 << PORTD6); // Turn on pull ups

    // Initialization of the USART 
    UCSRB |= (1 << TXEN);
    UCSRC |= (1 << UCSZ1) | (1 << UCSZ0); //Clock is divided by 1024 (prescalar)
    UBRRL = 2;
}

void sending_info(unsigned char data) {

    // This sends the data through the UART - to display on the tera term software
    while (!(UCSRA & (1 << UDRE)));
    UDR = data;
}

void converting_time(void) {
    // Function to convert Integer to ascii (numerical value)
    itoa(tenth, tenth_, 10);
    itoa(seconds, seconds_, 10);
    itoa(minutes, minutes_, 10);
    itoa(hours, hours_, 10);
    itoa(days, days_, 10);
}

void Interrupts(void) {
    // Interrupt is initialized
    sei();
}

void sending_time(void) {

    // All these for loops are created to send the individual components calculated in the timer, when the button is pressed

    for (int k = 0; k < sizeof (days_); k++) {
        sending_info(days_[k]);
    }
    sending_info(' ');

    for (int k = 0; k < sizeof (hours_); k++) {
        sending_info(hours_[k]);
    }

    sending_info(':');

    for (int k = 0; k < sizeof (minutes_); k++) {
        sending_info(minutes_[k]);
    }
    sending_info(':');

    for (int k = 0; k < sizeof (seconds_); k++) {
        sending_info(seconds_[k]);
    }
    sending_info(':');

    for (int k = 0; k < sizeof (tenth_); k++) {
        sending_info(tenth_[k]);
    }
}

void sending_nameid(void) {

    // For loop and character array to send the name and ID to the tera term
    char startup [] = "Pranlal 1628666 \r\n";
    for (int i = 0; i < sizeof (startup); i++) {
        sending_info(startup[i]);
    }
}

void buttons_config(void) {
    // Button configurations to avoid more inputs when the button is held and sends all the data (time and channel name)
    while (1) {
        if (!(PIND & (1 << PIND6))) {
            // Calls the convert time function to convert integer to ascii
            converting_time();
            // Sends the time when the button press was detected
            sending_time();

            // Shows which channel was pressed
            char message [] = "Channel 1 \r\n";
            for (int i = 0; i < sizeof (message); i++) {
                sending_info(message[i]);
            }

            // Holds the input, until the user lets go of the button 
            while (!(PIND & (1 << PIND6))) {
            };
        }

        if (!(PIND & (1 << PIND5))) {

            converting_time();
            sending_time();

            char message [] = "Channel 2 \n\r";
            for (int i = 0; i < sizeof (message); i++) {
                sending_info(message[i]);
            }

            while (!(PIND & (1 << PIND5))) {
            };

        }

        if (!(PIND & (1 << PIND4))) {

            converting_time();
            sending_time();

            char message [] = "Channel 3 \n\r";
            for (int i = 0; i < sizeof (message); i++) {
                sending_info(message[i]);
            }

            while (!(PIND & (1 << PIND4))) {
            };
        }

        if (!(PIND & (1 << PIND3))) {

            converting_time();
            sending_time();

            char message [] = "Channel 4 \n\r";
            for (int i = 0; i < sizeof (message); i++) {
                sending_info(message[i]);
            }

            while (!(PIND & (1 << PIND3))) {
            };
        }

    }
}



