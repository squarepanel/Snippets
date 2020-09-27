#define F_CPU 8000000UL  // clock speed

#include <avr/io.h>
#include <util/delay.h>

void init() {
    DDRC  = 0xFF;
    PORTC = 0x00;
    DDRD  = 0x00;  // PORTD as input

    DDRA  = 0x01;  // CSB
    PORTA = 0x01;  // CSB
}

void SPI_init( void ) {
    DDRB = ( ( 1 << DDB2 ) | ( 1 << DDB1 ) |
             ( 1 << DDB0 ) );  // spi pins on port b MOSI SCK,SS outputs
    SPCR = ( ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << CPOL ) |
             ( 1 << CPHA ) );  // SPI enable, Master, f/4
}

uint8_t SPI_Transmit( uint8_t cData ) {
    SPDR = cData;
    while( !( SPSR & ( 1 << SPIF ) ) )
        ;
    return SPDR;
}

// SPI transmit bits
#define RW 15
#define W1 14
#define W0 13

uint8_t spi_read( uint16_t address ) {
    address |= ( 1 << RW );  // read flag
    uint8_t spi_message[3] = { address >> 8, address & 0xFF, 0x00 };
    uint8_t data;
    PORTA &= ~0x01;  // clear CSB
    SPI_Transmit( spi_message[0] );
    SPI_Transmit( spi_message[1] );
    DDRB &= ~( 1 << DDB2 );  // disconnect mosi
    data = SPI_Transmit( spi_message[2] );
    DDRB |= ( 1 << DDB2 );  // connect mosi
    PORTA |= 0x01;          // set CSB
    return data;
}

void spi_write( uint16_t address, uint8_t data ) {
    uint8_t spi_message[3] = { address >> 8, address & 0xFF, data };
    PORTA &= ~0x01;  // clear CSB
    SPI_Transmit( spi_message[0] );
    SPI_Transmit( spi_message[1] );
    SPI_Transmit( spi_message[2] );
    PORTA |= 0x01;  // set CSB
}

int main( void ) {
    const uint8_t number_of_registers      = 18;
    uint8_t addresses[number_of_registers] = { 0x00, 0x01, 0x02, 0x04, 0x05,
                                               0xFF, 0x08, 0x09, 0x0D, 0x14,
                                               0x15, 0x16, 0x19, 0x1A, 0x1B,
                                               0x1C, 0x21, 0x22 };
    uint8_t default_values[number_of_registers] = {
        0x18, 0x09, 0b00110000, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x03,       0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    init();
    SPI_init();
    _delay_ms( 1000 );

    uint16_t         address = 0x0008;
    volatile uint8_t data;
    PORTC     = 0x00;
    uint8_t i = 0;
    while( 1 ) {
        // for( uint8_t i=0; i<18; ++i ){
        data  = spi_read( addresses[i] );
        PORTC = ( data == default_values[i] ) ? 0x01 : 0x00;
        // set pc0 if values are equal
        _delay_us( 10 );
        //}
        /* // power down
        spi_write( address, 0x00 );
        _delay_us(10);
        spi_write( 0x00FF, 0x01 );
        _delay_us(10);
        data = spi_read( address );
        */
        bool button_was_pressed = false;
        for( uint8_t d = 0; d < 255; d++ ) {
            if( ( PIND & 0x01 ) == 0 ) {
                d = 0;
                if( !button_was_pressed ) {
                    button_was_pressed = true;
                    ++i;
                    if( i == number_of_registers ) i = 0;
                }
            }
            _delay_ms( 1 );
        }
    }
}
