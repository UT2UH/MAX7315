/* Example usage for MAX7315 library.
 * Demonstrates the use of output of the MAX7315.
 *
 * You need to connect:
 * LED to MAX7315 IO pin 7
 */
#include <Arduino.h>
#include "MAX7315.h"

#define GPIO_PIN_LED     76  // LED connected to GPIO-0 as output
volatile uint8_t value = 0x70;

MAX7315 gpio;

void setup() {
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(400000);  
  gpio.begin();
  gpio.pinMode(GPIO_PIN_LED, OUTPUT);
  gpio.setIntensity(8, 0x80, HIGH);
  gpio.setIntensity(GPIO_PIN_LED, value, HIGH);
}

void loop() {
  gpio.setIntensity(GPIO_PIN_LED, value, HIGH);
  delay(250);
  value++;
  if(value >= 0x7F) value=0x70;
}
