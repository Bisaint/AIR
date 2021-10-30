/*
        ADS1256.h - Arduino Library for communication with Texas Instrument ADS1256 ADC
        Written by Adien Akhmad, August 2015
	Modifified  Jan 2019 by Axel Sepulveda for ATMEGA328
*/

#ifndef ADS1256_h
#define ADS1256_h

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
	// Define PORT
	#define PORT_DRDY PORTB // Pin 9 on Arduino UNO
	#define PIN_DRDY PINB
	#define PINDEX_DRDY PB1
	#define DDR_DRDY DDRB

	#define PORT_CS PORTB // Pin 10 on Arduino UNO
	#define PIN_CS PINB
	#define PINDEX_CS PB2
	#define DDR_CS DDRB

	#define PORT_RESET PORTB // PIN 8 on Arduino UNO
	#define PIN_REST PINB
	#define PINDEX_RESET PB0
	#define DDR_RESET DDRB

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	// Define PORT
	#define PORT_DRDY PORTL // Pin 49 on Arduino Mega
	#define PIN_DRDY PINL
	#define PINDEX_DRDY PL0
	#define DDR_DRDY DDRL

	#define PORT_CS PORTB // Pin 53 on Arduino Mega
	#define PIN_CS PINB
	#define PINDEX_CS PB0
	#define DDR_CS DDRB

	#define PORT_RESET PORTL // PIN 48 on Arduino Mega
	#define PIN_REST PINL
	#define PINDEX_RESET PL1
	#define DDR_RESET DDRL
	
	// Contributions are welcome
#elif   defined(ARDUINO_ARCH_ESP32)
	#error "Oops! ESP32 architecture not supported yet"	
	// Contributions are welcome
#else 
	// Contributions are welcome
	#error "Oops! Your board architecture is not supported yet'"
#endif
// ADS1256 Register
#define STATUS 0x00
#define MUX 0x01
#define ADCON 0x02
#define DRATE 0x03
#define IO 0x04
#define OFC0 0x05
#define OFC1 0x06
#define OFC2 0x07
#define FSC0 0x08
#define FSC1 0x09
#define FSC2 0x0A

// ADS1256 Command
#define WAKEUP 0x00
#define RDATA 0x01
#define RDATAC 0x03
#define SDATAC 0x0f
#define RREG 0x10
#define WREG 0x50
#define SELFCAL 0xF0
#define SELFOCAL 0xF1
#define SELFGCAL 0xF2
#define SYSOCAL 0xF3
#define SYSGCAL 0xF4
#define SYNC 0xFC
#define STANDBY 0xFD
#define RESET 0xFE

// define multiplexer codes
#define ADS1256_MUXP_AIN0 0x00
#define ADS1256_MUXP_AIN1 0x10
#define ADS1256_MUXP_AIN2 0x20
#define ADS1256_MUXP_AIN3 0x30
#define ADS1256_MUXP_AIN4 0x40
#define ADS1256_MUXP_AIN5 0x50
#define ADS1256_MUXP_AIN6 0x60
#define ADS1256_MUXP_AIN7 0x70
#define ADS1256_MUXP_AINCOM 0x80

#define ADS1256_MUXN_AIN0 0x00
#define ADS1256_MUXN_AIN1 0x01
#define ADS1256_MUXN_AIN2 0x02
#define ADS1256_MUXN_AIN3 0x03
#define ADS1256_MUXN_AIN4 0x04
#define ADS1256_MUXN_AIN5 0x05
#define ADS1256_MUXN_AIN6 0x06
#define ADS1256_MUXN_AIN7 0x07
#define ADS1256_MUXN_AINCOM 0x08

// define gain codes
#define ADS1256_GAIN_1 0x00
#define ADS1256_GAIN_2 0x01
#define ADS1256_GAIN_4 0x02
#define ADS1256_GAIN_8 0x03
#define ADS1256_GAIN_16 0x04
#define ADS1256_GAIN_32 0x05
#define ADS1256_GAIN_64 0x06

// define drate codes
/*
        NOTE : 	Data Rate vary depending on crystal frequency. Data rates
   listed below assumes the crystal frequency is 7.68Mhz
                for other frequency consult the datasheet.
*/

#define ADS1256_DRATE_30000SPS 0xF0
#define ADS1256_DRATE_15000SPS 0xE0
#define ADS1256_DRATE_7500SPS 0xD0
#define ADS1256_DRATE_3750SPS 0xC0
#define ADS1256_DRATE_2000SPS 0xB0
#define ADS1256_DRATE_1000SPS 0xA1
#define ADS1256_DRATE_500SPS 0x92
#define ADS1256_DRATE_100SPS 0x82
#define ADS1256_DRATE_60SPS 0x72
#define ADS1256_DRATE_50SPS 0x63
#define ADS1256_DRATE_30SPS 0x53
#define ADS1256_DRATE_25SPS 0x43
#define ADS1256_DRATE_15SPS 0x33
#define ADS1256_DRATE_10SPS 0x23
#define ADS1256_DRATE_5SPS 0x13
#define ADS1256_DRATE_2_5SPS 0x03

#include "Arduino.h"
#include "SPI.h"

class ADS1256 {
 public:
  ADS1256(float clockspdMhz, float vref, bool useresetpin);
  void writeRegister(unsigned char reg, unsigned char wdata);
  unsigned char readRegister(unsigned char reg);
  void sendCommand(unsigned char cmd);
  float readCurrentChannel();
  void setConversionFactor(float val);
  void setChannel(byte channel);
  void setChannel(byte AIP, byte AIN);
  void begin(unsigned char drate, unsigned char gain, bool bufferenable);
  void waitDRDY();
  void setGain(uint8_t gain);
  void readTest();

 private:
  void CSON();
  void CSOFF();
  unsigned long read_uint24();
  long read_int32();
  float read_float32();
  byte _pga;
  float _VREF;
  float _conversionFactor;
};

#endif
