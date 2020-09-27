#ifndef INCLUDE_UTILS_H
#define INCLUDE_UTILS_H

uint16_t reverse_2bytes( uint16_t data ) {
    uint16_t reversed_data = 0x0000;
    for( uint8_t i = 0; i < 16; ++i ) {
        reversed_data <<= 1;
        reversed_data |= data & 1;
        data >>= 1;
    }
    return reversed_data;
}

uint8_t reverse_byte( uint8_t data ) {
    uint8_t reversed_data = 0x00;
    for( uint8_t i = 0; i < 8; ++i ) {
        reversed_data <<= 1;
        reversed_data |= data & 1;
        data >>= 1;
    }
    return reversed_data;
}


#define SET_BIT( register, bit )   register |= ( 1 << bit )
#define CLEAR_BIT( register, bit ) register &= ~( 1 << bit )

#endif
