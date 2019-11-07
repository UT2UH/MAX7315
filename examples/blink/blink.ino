/* Example usage for MAX7315 library.
 * Demonstrates the use of output of the MAX7315.
 *
 * You need to connect:
 * LED to MAX7315 IO pin 4
 */
#include <Wire.h>
#include "MAX7315.h"

#define GPIO_PIN_LED           4  // LED connected to GPIO-4 as output

MAX7315 gpio;

void setup() {
  Wire.begin();
  Wire.setClock(400000);
  gpio.begin();
  gpio.pinMode(GPIO_PIN_LED, OUTPUT);
}

void loop() {
  gpio.digitalWrite(GPIO_PIN_LED, LOW); // LED On
  delay(500);
  gpio.digitalWrite(GPIO_PIN_LED, HIGH); // LED Off
  delay(500);
}
