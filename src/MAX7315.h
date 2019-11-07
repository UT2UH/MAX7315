/* MAX7315 library 
 * Heavily based on PCA9534 library by Abdulrahman Saleh Khamis
 *
 * This library allows for easy interfacing with MAX7315 GPIO expander chip
 * using I2C protocol. It has familiar functions when dealing with the pins
 * such as pinMode(), digitalWrite(), and digitalRead().
 *
 * It also allows using interrupt when input status changes,
 * to avoid continuous polling.
 */

#ifndef MAX7315_H
  #define MAX7315_H

  #if (ARDUINO >= 100)
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

  #include <Wire.h>

  #define MAX7315_I2C_ADDRESS    0x20
  #define MAX7315_READ_INPUT     0x00
  #define MAX7315_BLINK_PHASE0   0x01

  #define MAX7315_PORTS_CONFIG   0x03

  #define MAX7315_BLINK_PHASE1   0x09

  #define MAX7315_INTENSITY_O8   0x0E
  #define MAX7315_CONFIGURATION  0x0F
  #define MAX7315_INTENSITY_P1P0 0x10
  #define MAX7315_INTENSITY_P3P2 0x11
  #define MAX7315_INTENSITY_P5P4 0x12
  #define MAX7315_INTENSITY_P7P6 0x13

class MAX7315 {
public:
  /**
   * Constructor
   * Creates a new MAX7315 class to manage a MAX7315 chip.
   */
  MAX7315();

  /**
   * Initializes the device and performs initial I2C setup.
   * This method should be called before any others are used.
   *
   * @param {uint8_t} i2caddr - Sets the slave address of the MAX7315,
   * defaults to 0x20.
   */
  void begin(TwoWire &wirePort = Wire, uint8_t i2caddr = MAX7315_I2C_ADDRESS);

  /**
   * Configures the specified pin to behave either as an input, inverted input,
   * or output.
   *
   * @param {uint8_t} pin - Pin number whose mode you wish to set.
   * @param {uint8_t} mode - Pin mode one of: INPUT, or OUTPUT.
   */
  void pinMode(uint8_t pin, uint8_t mode);

  /**
   * Reads the value from a specified digital pin, either HIGH or LOW.
   *
   * @param {uint8_t} pin - Pin number whose value you wish to get.
   * @returns {uint8_t} The status of the pin either HIGH or LOW.
   */
  uint8_t digitalRead(uint8_t pin);
  
  /**
   * Writes a HIGH or a LOW value to a digital pin.
   *
   * @param {uint8_t} pin - Pin number whose value you wish to set.
   * @param {uint8_t} value - Pin value one of: HIGH, or LOW.
   */
  void digitalWrite(uint8_t pin, uint8_t value);
  

private:
  TwoWire *_i2cPort;//The generic connection to user's chosen I2C hardware
  uint8_t _i2caddr; // I2C address of the device
  uint8_t _port;    // Port configuration status on Configuration register
  uint8_t _leds;	// Ports with PWM enabled
};

#endif