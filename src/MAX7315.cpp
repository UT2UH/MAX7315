#include "MAX7315.h"

MAX7315::MAX7315() {
  _i2caddr = 0x20;
  _port = 0;
  _leds = 0;
}

void MAX7315::begin(TwoWire &wirePort, uint8_t i2caddr) {
  //commType = COMM_TYPE_I2C;
  _i2cPort = &wirePort; //Grab which port the user wants us to use

  //We expect caller to begin their I2C port, with the speed of their choice external to the library
  //But if they forget, we start the hardware here.

  //We're moving away from the practice of starting Wire hardware in a library. This is to avoid cross platform issues.
  //ie, there are some platforms that don't handle multiple starts to the wire hardware. Also, every time you start the wire
  //hardware the clock speed reverts back to 100kHz regardless of previous Wire.setClocks().
  //_i2cPort->begin();

  _i2caddr = i2caddr; //Store the I2C address from user  
}

void MAX7315::pinMode(uint8_t pin, uint8_t mode) {
  switch (mode) {
    case OUTPUT:
      // Clear the pin on the configuration register for output
      _port &= ~(1 << pin);
      break;
    case INPUT:
    default:
      // Set the pin on the configuration register for input
      _port |= (1 << pin);
      break;
  }

  // Write the configuration of the individual pins as inputs or outputs
  _i2cPort->beginTransmission(_i2caddr);
  _i2cPort->write(MAX7315_PORTS_CONFIG);
  _i2cPort->write(_port);
  _i2cPort->endTransmission();
}

uint8_t MAX7315::digitalRead(uint8_t pin) {
  uint8_t buff = 0;

  // We need to write to the input register first to get the status of the pins.
  // As per the specs: "a write transmission is sent with the command byte to
  // let the I2C device know that the Input Port register will be accessed
  // next."
  _i2cPort->beginTransmission(_i2caddr);
  _i2cPort->write(MAX7315_READ_INPUT);
  _i2cPort->write(_port);
  _i2cPort->endTransmission();

  // We only need to read 1 byte of data to get the pins
  _i2cPort->requestFrom((uint8_t)_i2caddr, (uint8_t)1);
  while (_i2cPort->available()) {
    buff = _i2cPort->read();
  }
  return (buff & (1 << pin)) ? HIGH : LOW;
}

void MAX7315::digitalWrite(uint8_t pin, uint8_t value) {
  switch (value) {
    case HIGH:
      // Set the pin HIGH on the output register
      _port |= (1 << pin);
      break;
    case LOW:
    default:
      // Set the pin LOW on the output register
      _port &= ~(1 << pin);
      break;
  }
  // Write the status of the pins on the output register
  _i2cPort->beginTransmission(_i2caddr);
  _i2cPort->write(MAX7315_BLINK_PHASE0);
  _i2cPort->write(_port);
  _i2cPort->endTransmission();
}