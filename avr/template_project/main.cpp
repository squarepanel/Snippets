#define F_CPU 8000000UL  // clock speed

#include <avr/io.h>
#include <util/delay.h>

#include "avr_utils.hpp"

#define PINS_DECLARATION                                                       \
    Pin Button_DN( PORTG, 0 );                                                 \
    Pin Button_OK( PORTG, 1 );                                                 \
    Pin Button_UP( PORTG, 2 );                                                 \
    Pin portl0( PORTL, 0 );                                                    \
    Pin portl1( PORTL, 1 );                                                    \
    Pin portl2( PORTL, 2 );                                                    \
    Pin portl3( PORTL, 3 );                                                    \
    Pin portl4( PORTL, 4 );                                                    \
    Pin portl5( PORTL, 5 );                                                    \
    Pin portl6( PORTL, 6 );                                                    \
    Pin portl7( PORTL, 7 );

void board_init() {
    DDRL  = 0xFF;
    PORTL = 0x00;  // LED
}

void test_init() {}

void btn_dn_routine() {
    PINS_DECLARATION
    toggle( portl0 );
}
void btn_ok_routine() {
    PINS_DECLARATION
    toggle( portl1 );
}
void btn_up_routine() {
    PINS_DECLARATION
    toggle( portl2 );
}

int main() {
    PINS_DECLARATION
    board_init();
    test_init();


    BUTTON_PROCESSING_LOOP_START();
    PROCESS_BUTTON( IS_LOW( Button_UP ), btn_up_routine );
    PROCESS_BUTTON( IS_LOW( Button_OK ), btn_ok_routine );
    PROCESS_BUTTON( IS_LOW( Button_DN ), btn_dn_routine );
    BUTTON_PROCESSING_LOOP_END();
    return 0;
}
