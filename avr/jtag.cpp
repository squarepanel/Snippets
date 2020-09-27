
#define JTAG_PORT PORTJ
#define JTAG_PIN  PINJ

#define TRST 0
#define TCK  1
#define TMS  2
#define TDI  3
#define TDO  4

#define JTAG_INSTRUCTION_SIZE  8
#define JTAG_TDO_BUFFER_LENGTH 4
// const uint32 expexted_data = { 0x0970403f };
const uint8_t expected_data[JTAG_TDO_BUFFER_LENGTH] = { 0x3F, 0x40, 0x70,
                                                        0x09 };  // Atmega1281

inline void jtag_tick( uint8_t jtag_buffer ) {
    jtag_buffer &= ~( 1 << TCK );
    JTAG_PORT = jtag_buffer;
    _delay_us( 1 );
    jtag_buffer |= ( 1 << TCK );
    JTAG_PORT = jtag_buffer;
    _delay_us( 1 );
}

inline void jtag_tick_and_read( uint8_t jtag_buffer, uint8_t& out_data ) {
    out_data <<= 1;

    JTAG_PORT = jtag_buffer & ~( 1 << TCK );

    _delay_us( 1 );

    JTAG_PORT         = jtag_buffer;
    uint8_t temp_data = ( JTAG_PIN >> TDO ) & 0x01;

    out_data |= temp_data;
}

bool jtag( uint8_t instruction ) {
    uint8_t       jtag_buffer = ( 1 << TRST ) | ( 1 << TCK );
    const uint8_t tms         = 1 << TMS;

    uint8_t data[JTAG_TDO_BUFFER_LENGTH];
    for( uint8_t i = 0; i < JTAG_TDO_BUFFER_LENGTH; ++i ) data[i] = 0x00;

    for( uint8_t i = 0; i < 5; ++i )
        jtag_tick( jtag_buffer | tms );  // TMS reset

    jtag_tick( jtag_buffer );
    jtag_tick( jtag_buffer | tms );  // select DR scan
    jtag_tick( jtag_buffer | tms );  // select IR scan
    jtag_tick( jtag_buffer );        // capture IR
    jtag_tick( jtag_buffer );        // shift IR

    for( uint8_t i = 0; i < JTAG_INSTRUCTION_SIZE - 1; ++i ) {
        jtag_tick( jtag_buffer |
                   ( ( instruction & 0x01 ) << TDI ) );  // shift IR
        instruction >>= 1;
    }  // the last bit goes with tms
    jtag_tick( jtag_buffer |
               ( ( ( instruction & 0x01 ) << TDI ) | tms ) );  // Exit1 IR
    jtag_tick( jtag_buffer | tms );                            // Update IR
    jtag_tick( jtag_buffer );                                  // idle

    jtag_tick( jtag_buffer | tms );  // select DR scan
    jtag_tick( jtag_buffer );        // capture DR
    jtag_tick( jtag_buffer );        // shift DR

    for( uint8_t i = 0; i < 8; ++i ) jtag_tick_and_read( jtag_buffer, data[0] );
    for( uint8_t i = 0; i < 8; ++i ) jtag_tick_and_read( jtag_buffer, data[1] );
    for( uint8_t i = 0; i < 8; ++i ) jtag_tick_and_read( jtag_buffer, data[2] );
    for( uint8_t i = 0; i < 7; ++i ) jtag_tick_and_read( jtag_buffer, data[3] );
    jtag_tick_and_read( jtag_buffer | tms,
                        data[3] );  // the last bit goes with tms

    for( uint8_t i = 0; i < 5; ++i )
        jtag_tick( jtag_buffer | tms );  // TMS reset
    for( uint8_t i = 0; i < JTAG_TDO_BUFFER_LENGTH; ++i ) {
        data[i] = reverse_byte( data[i] );
    }

    JTAG_PORT = 0x0f;
    for( uint8_t i = 0; i < JTAG_TDO_BUFFER_LENGTH; ++i )
        if( data[i] != expected_data[i] ) return false;
    return true;
}
