#include <U8glib.h>

// setup u8g object
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 

/* SLboat Add Device */
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C 128x64(col2-col129) SH1106,Like HeiTec 1.3' I2C OLED 

struct screen {
  char items[4][14];
};
screen main_menu = { "LED", "Serial", "Bluetooth", "Setting" };
screen bluetooth_menu = { "Bluetooth", "OFF", "Enable", "Return" };
screen* active_menu = &main_menu;
uint8_t active_ui_item = 0;

void draw( char* str ) {
  // graphic commands to redraw the complete screen should be placed here  
  //u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  const uint8_t vertical_step = 12;
  for( uint8_t i = 0; i < 4; ++i ){
    uint8_t y = i*(vertical_step+2) + vertical_step;
    if( active_ui_item == i ) u8g.drawStr( 0, y, ">" );  
    u8g.drawStr( 8, y, active_menu->items[i] );  
    //u8g.drawStr( 120, vertical_step*(i+1), "]" );  
  }
  //u8g.drawStr( 88, 4*(vertical_step+2) + vertical_step, str );
}

const uint8_t buttonPin = 3;     // the number of the pushbutton pin
const uint8_t BT_switch = 7;
const uint8_t ledPin =  13;      // the number of the LED pin

// IR code
#define IRpin          2
// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 675
// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 100
// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[100][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing

bool transfer = false;
// *****
char data = 0;                //Variable for storing received data rx/tx from BT


void setup(void) {
  Serial.begin(9600);
  Serial.println("CORNTACT!");
  
  // flip screen, if required
  // u8g.setRot180();
  u8g.setFont(u8g_font_unifont);
  u8g.setColorIndex(1);     // white

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(BT_switch, OUTPUT );
  PORTD &= !( 1 << BT_switch );

  updatescreen(0);
}

void transfer_completion(){
  printpulses();
  currentpulse=0;
  transfer = false;
}
void loop(void) {
  uint32_t highpulse, lowpulse;  // temporary storage timing
  highpulse = lowpulse = 0; // start out with no pulse length
  char str[16] = "                ";

  if(Serial.available() > 0)  // Send data only when you receive data:
  {
    data = Serial.read();      //Read the incoming data and store it into variable data
    Serial.print(data);        //Print Value inside data in Serial monitor
    Serial.print("\n");        //New line 
    if(data == '1')            //Checks whether value of data is equal to 1 
    digitalWrite(13, HIGH);  //If value is 1 then LED turns ON
    else if(data == '0')       //Checks whether value of data is equal to 0
    digitalWrite(13, LOW);   //If value is 0 then LED turns OFF
  }        

  if( !transfer ){ 
    if( !( PIND & (1 << IRpin) ) ){
      transfer = true;
      return;
    } else {
      delayMicroseconds( RESOLUTION );
    }
  } else {
    while (! (PIND & (1 << IRpin))) {
       // pin is still LOW
       lowpulse++;
       delayMicroseconds(RESOLUTION);
       if ((lowpulse >= MAXPULSE) && (currentpulse != 0)) {
         transfer_completion();
         return;
       }
    }
    pulses[currentpulse][0] = lowpulse;
    
    while (PIND & (1 << IRpin)) {
   
       // count off another few microseconds
       highpulse++;
       delayMicroseconds(RESOLUTION);
   
       if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
         transfer_completion();
         return;
       }
    }
    pulses[currentpulse][1] = highpulse;
      // we read one high-low pulse successfully, continue!
    currentpulse++;
  }
}


void updatescreen(uint8_t code) {
  /*
  int corntact = digitalRead(buttonPin);
  if ( corntact == HIGH ) digitalWrite( ledPin, LOW );
  else digitalWrite( ledPin, HIGH );
  */
  char* command;
  switch(code){
    case 0:   command = "";   break;
    case 104: command = "1";  break;
    case 152: command = "2";  break;
    case 176: command = "3";  break;
    case  48: command = "4";  break;
    case  24: command = "5";  break;
    case 122: command = "6";  break;
    case  16: command = "7";  break;
    case  56: command = "8";  break;
    case  90: command = "9";  break;
    case  66: command = "*";  break;
    case  74: command = "0";  break;
    case  82: command = "#";  break;
    case  34: command = "left";   break;
    case  98: command = "up";   
      if( active_ui_item == 0 ) active_ui_item = 3;
      else active_ui_item--;
    break;
    case 194: command = "right";  break;
    case 168: command = "down";  
      if( active_ui_item == 3 ) active_ui_item = 0;
      else active_ui_item++;
    break;
    case   2: command = "ok";
      switch( active_ui_item ){
        case 0: 
          PORTB ^= 1 << 5;
        break;
        case 1: break;
        case 2: 
          if( active_menu == &main_menu ) active_menu = &bluetooth_menu; 
          else{ 
            bluetooth_menu.items[1][0] = 'O';
            bluetooth_menu.items[1][1] = 'N';
            bluetooth_menu.items[1][2] = ' ';
            PORTD |= 1 << BT_switch;
          }
          break;
        case 3: break;
      } 
    break;
  }
  
  u8g.firstPage();  
  do {
    draw( command );
  } while( u8g.nextPage() );
}

void printpulses(void) { 
  
  // print it in a 'array' format
  Serial.println("cornact IR {");

  uint8_t pulse = 0;
  if( pulses[pulse][0] > 80 && pulses[pulse][0] < 100 && 
      pulses[pulse][1] > 40 && pulses[pulse][1] < 50 ){
    uint8_t address = 0, address_inv = 0, data = 0, data_inv = 0;
    for( uint8_t i = 1; i < 9; ++i ){
      if( pulses[i][0] >= 5 && pulses[i][0] <= 7 ){
        if( pulses[i][1] >= 5 && pulses[i][1] <= 7 ){
          address = (address << 1) + 0;
        }else if ( pulses[i][1] >= 15 && pulses[i][1] <= 18 ){
          address = (address << 1) + 1;
        }else Serial.println("Error! address");
      }
    }
    Serial.println(address, DEC);
    for( uint8_t i = 9; i < 17; ++i ){
      if( pulses[i][0] >= 5 && pulses[i][0] <= 7 ){
        if( pulses[i][1] >= 5 && pulses[i][1] <= 7 ){
          address_inv = (address_inv << 1) + 0;
        }else if ( pulses[i][1] >= 15 && pulses[i][1] <= 18 ){
          address_inv = (address_inv << 1) + 1;
        }else Serial.println("Error! address_inv");
      }
    }
    Serial.println(address_inv, DEC);
    for( uint8_t i = 17; i < 25; ++i ){
      if( pulses[i][0] >= 5 && pulses[i][0] <= 7 ){
        if( pulses[i][1] >= 5 && pulses[i][1] <= 7 ){
          data = (data << 1) + 0;
        }else if ( pulses[i][1] >= 15 && pulses[i][1] <= 18 ){
          data = (data << 1) + 1;
        }else Serial.println("Error! data");
      }
    }
    Serial.println(data, DEC);
    updatescreen(data);
    for( uint8_t i = 25; i < 33; ++i ){
      if( pulses[i][0] >= 5 && pulses[i][0] <= 7 ){
        if( pulses[i][1] >= 5 && pulses[i][1] <= 7 ){
          data_inv = (data_inv << 1) + 0;
        }else if ( pulses[i][1] >= 15 && pulses[i][1] <= 18 ){
          data_inv = (data_inv << 1) + 1;
        }else Serial.println("Error! data_inv");
      }
    }
    Serial.println(data_inv, DEC);
  }else return;
  

  /*
  for (uint8_t i = 0; i <= currentpulse; i++) {
    Serial.print("burst(");
    Serial.print(pulses[i][0] * RESOLUTION , DEC);
    Serial.print(");\t");
    
    Serial.print("space(");
    Serial.print(pulses[i][1] * RESOLUTION , DEC);
    Serial.println(");");
  }*/
}
