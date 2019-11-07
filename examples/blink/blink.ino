/* Example usage for MAX7315 library.
 * Demonstrates the use of output of the MAX7315.
 *
 * You need to connect:
 * LED to MAX7315 IO pin 0
 */

#include "MAX7315.h"

#define GPIO_PIN_LED           0  // LED connected to GPIO-0 as output

MAX7315 gpio;

// Helps ensure the application loop is not interrupted by the system
// background processing and network management.
SYSTEM_THREAD(ENABLED);

void setup() {
  gpio.begin();
  gpio.pinMode(GPIO_PIN_LED, OUTPUT);
}

void loop() {
  gpio.digitalWrite(GPIO_PIN_LED, LOW); // LED On
  delay(500);
  gpio.digitalWrite(GPIO_PIN_LED, HIGH); // LED Off
  delay(500);
}
