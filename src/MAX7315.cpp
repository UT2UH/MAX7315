#include "MAX7315.h"

MAX7315::MAX7315() {
  _i2caddr  = 0x20;
  _port     = 0xFF;
  _phase0   = 0xFF;
  _phase1   = 0xFF;
  _masterO8 = 0x0F;
  _config   = 0x0C;
  _intensVal[4];
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
      _phase0 |= (1 << pin);
      break;
    case LOW:
    default:
      // Set the pin LOW on the output register
      _phase0 &= ~(1 << pin);
      break;
  }
  // Write the status of the pins on the output register
  _i2cPort->beginTransmission(_i2caddr);
  _i2cPort->write(MAX7315_BLINK_PHASE0);
  _i2cPort->write(_phase0);
  _i2cPort->endTransmission();
}

// Valid pin numbers - 0-8, 10, 32, 54, 76
void MAX7315::setIntensity(uint8_t pin, uint8_t value, uint8_t phase) {
  uint8_t intensReg = 0;  
  uint8_t intensity = 0;
  uint8_t offset    = 0;
  uint8_t mask      = 0;

  // Put value's MSB to MasterO8 register
  _masterO8   = (_masterO8 & 0x0F) | (value & 0xF0);

  if (pin <= 7) {                            // Pins 0-7
    // Clear the pin on the configuration register for output
    mask = ~(1 << pin);
    // Choose intensity register  
    offset = (pin >> 1);     
    // Put value's 4 LSB to Output intensity register
    if ((pin & 1) == 1) {
      _intensVal[offset] = (_intensVal[offset] & 0x0F) | (value << 4);      //odd      
    } else {
      _intensVal[offset] = (_intensVal[offset] & 0xF0) | (value & 0x0F);    //0,2,4,6 
    }

  } else if (pin == 8) {                      // Pin O8
    // Put value's 4 LSB to O8/Global intensity register
    _masterO8 = (_masterO8 & 0xF0) | (value & 0x0F);
    intensity = _intensVal[0];
    offset = 0;
    _config &= 0xF7;                          // O8 is not Int
  } else {                                    // Pin pairs: 10, 32, 54, 76
    intensity = (value << 4) | (value & 0x0F); //duplicate four LSB value  
    switch (pin) {
      case 10:
        mask = 0xFC;
        offset = 0;
        break;
      case 32:
        mask = 0xF3;
        offset = 1;           
        break;    
      case 54:
        mask = 0xCF;
        offset = 2;          
        break;    
      case 76:    
        mask = 0x3F;
        offset = 3;          
        break;
      default:
        mask = 0xFF;                       //Do not change anything TODO return eror
        offset = 0;
        intensity = _intensVal[0];
        break;
    }
    _intensVal[offset] = intensity;
  }

  intensity = _intensVal[offset];
  intensReg = MAX7315_INTENSITY_P1P0 + offset;
  if(phase == LOW ){
    _phase0 &= mask;                    // Increase output(s) PWM intensity with @value increasing
  } else {
    _phase0 |= ~mask;
  }
  _config &= 0xF8;                   // Disable Blink and Global Intensity
  _port &= mask;

  // Write the status of the pins on the output register
  _i2cPort->beginTransmission(_i2caddr);
  _i2cPort->write(MAX7315_PORTS_CONFIG);
  _i2cPort->write(_port);
  _i2cPort->endTransmission();

  _i2cPort->beginTransmission(_i2caddr);
  _i2cPort->write(MAX7315_INTENSITY_O8);
  _i2cPort->write(_masterO8);             // enable OSC and set Master Intensity to 15/15
  _i2cPort->endTransmission();
  
  _i2cPort->beginTransmission(_i2caddr);  
  _i2cPort->write(MAX7315_CONFIGURATION);
  _i2cPort->write(_config);                 //Disable Blink, Set individual intensity control
  _i2cPort->endTransmission();

  _i2cPort->beginTransmission(_i2caddr);  
  _i2cPort->write(MAX7315_BLINK_PHASE0);
  _i2cPort->write(_phase0);                 //PWM time greatest when value in PWMIntensity is low when BlinkPhase0 is 1s
  _i2cPort->endTransmission();

  _i2cPort->beginTransmission(_i2caddr);  
  _i2cPort->write(intensReg);  
  _i2cPort->write(intensity);             //Initially all lights half intensity  now on off
  _i2cPort->endTransmission();
  /*
  Serial.print(pin,DEC); Serial.print(" ");
  Serial.print(mask,HEX); Serial.print(" ");
  Serial.print(value,HEX); Serial.print(" ");
  Serial.print(_masterO8,HEX); Serial.print(" ");
  Serial.print(_phase0,HEX); Serial.print("  ");
  Serial.print(_intensVal[3],HEX); Serial.print(" ");  
  Serial.print(_intensVal[2],HEX); Serial.print(" "); 
  Serial.print(_intensVal[1],HEX); Serial.print(" ");
  Serial.println(_intensVal[0],HEX);
  */
}