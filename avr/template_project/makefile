MCU = atmega2560
TARGET = test

CXX      = avr-g++
OBJCOPY  = avr-objcopy
WARNINGS = -Wall -Wextra -Weffc++ -pedantic	\
           -Wshadow -Wcast-align -Wunreachable-code -Wswitch-enum
CMDFLAGS = -fno-diagnostics-color
CPPFLAGS = $(WARNINGS) $(CMDFLAGS) -O2 -std=c++17 -mmcu=$(MCU) -fno-exceptions \
		   -IC:\avr-gcc-9.2.0-x86-mingw\avr\include
LDFLAGS  =
OBJS     = main.o

build: $(TARGET).hex
$(TARGET).hex : $(OBJS)
	$(CXX) $(CPPFLAGS) -o $(TARGET).bin $^
	$(OBJCOPY) -O ihex $(TARGET).bin $(TARGET).hex
	sizeof $(TARGET).hex

%.o: %.cpp %.hpp %.h
	$(CXX) -c $<

load: build
	atprogram -t atmelice -i jtag -d $(MCU) program --chiperase --verify -f "$(TARGET).hex"

erase:
	atprogram -t atmelice -i jtag -d $(MCU) chiperase

clean:
	rm -f core $(TARGET).bin $(TARGET).hex $(OBJS)

rebuild: clean build
