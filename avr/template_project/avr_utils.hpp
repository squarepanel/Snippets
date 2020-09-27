#ifndef INCLUDE_AVR_UTILS_HPP
#define INCLUDE_AVR_UTILS_HPP

template <typename T>
constexpr T reverse( T data ) {
    T reversed_data = 0x00;
    for( uint8_t i = 0; i < sizeof( T ) * 8; ++i ) {
        reversed_data <<= 1;
        reversed_data |= data & 1;
        data >>= 1;
    }
    return reversed_data;
}

template <typename P, typename T>
constexpr void set_bit( P& port, T bit ) {
    port |= ( 1 << bit );
}
template <typename P, typename T>
constexpr void clear_bit( P& port, T bit ) {
    port &= ~( 1 << bit );
}
template <typename P, typename T>
void toggle_bit( P& port, T t ) {
    port ^= ( 1 << t );
}

template <typename P, typename T>
class Pin {
    /*
        void togle_pulse ( Pin& pin, time );
        ? pulse_low
        ? pulse_high
    */
  public:
    P* const t_port;
    const T  pin;
    Pin( P& port, T pin_num ) : t_port( &port ), pin( pin_num ) {}
    void set() { set_bit( *t_port, pin ); }
    void clear() { clear_bit( *t_port, pin ); }
    void toggle() { toggle_bit( *t_port, pin ); }
    bool is_low() const { return ( ( *( t_port - 2 ) ) & ( 1 << pin ) ) == 0; }
    bool is_high() const { return ( ( *( t_port - 2 ) ) & ( 1 << pin ) ) != 0; }
         operator T() { return pin; }
};
auto set    = []( auto& p ) { p.set(); };
auto clear  = []( auto& p ) { p.clear(); };
auto toggle = []( auto& p ) { p.toggle(); };

#define IS_LOW( pin_name )                                                     \
    ( ( ( *( pin_name.t_port - 2 ) ) & ( 1 << pin_name.pin ) ) == 0 )
#define IS_HIGH( pin_name )                                                    \
    ( ( ( *( pin_name.t_port - 2 ) ) & ( 1 << pin_name.pin ) ) != 0 )

template <typename P, typename... T>
void set_bits( P& port, T... t ) {
    port |= ( ( 1 << t ) | ... );
}
template <typename P, typename... T>
void clear_bits( P& port, T... t ) {
    port &= ~( ( 1 << t ) | ... );
}

template <typename P, typename... T>
void toggle_bits( P& port, T... t ) {
    port ^= ( ( 1 << t ) | ... );
}

// SUPER UNSAFE STUFF DOWN HERE
#define BUTTON_PROCESSING_LOOP_START()                                         \
    while( 1 ) {                                                               \
        bool button_was_pressed = false;                                       \
        for( uint8_t d = 0; d < 255; ++d ) {
#define PROCESS_BUTTON( CONDITION, CALLBACK )                                  \
    if( CONDITION ) {                                                          \
        d = 0;                                                                 \
        if( !button_was_pressed ) {                                            \
            button_was_pressed = true;                                         \
            CALLBACK();                                                        \
        }                                                                      \
    }
#define BUTTON_PROCESSING_LOOP_END()                                           \
    }                                                                          \
    }
/*
while( 1 ) {
    bool button_was_pressed = false;
    for( uint8_t d = 0; d < 255; ++d ) {
        if( IS_LOW( Button_DN ) ) {
            d = 0;
            if( !button_was_pressed ) {
                button_was_pressed = true;
                btn_dn_routine();
            }
        }
        if( IS_LOW( Button_OK ) ) {
            d = 0;
            if( !button_was_pressed ) {
                button_was_pressed = true;
                btn_ok_routine();
            }
        }
        if( IS_LOW( Button_UP ) ) {
            d = 0;
            if( !button_was_pressed ) {
                button_was_pressed = true;
                btn_up_routine();
            }
        }
    }
}
*/
#endif
