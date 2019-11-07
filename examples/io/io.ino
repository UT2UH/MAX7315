/* Example usage for MAX7315 library.
 * Demonstrates the use of simple input/output functionality of the MAX7315.
 *
 * You need to connect:
 * LED to MAX7315 IO pin 0
 * Button to MAX7315 IO pin 1
 */

#include "MAX7315.h"

#define GPIO_PIN_LED           0  // LED connected to GPIO-0 as output
#define GPIO_PIN_BUTTON        1  // Button connected to GPIO-1 as input

MAX7315 gpio;
// Last time the output pin was toggled
unsigned long lastDebounceTime = 0;
// Debounce time; increase if the output flickers
unsigned long debounceDelay = 25;
// Default button status for debouncing
uint8_t lastButtonStatus = HIGH;

// Helps ensure the application loop is not interrupted by the system
// background processing and network management.
SYSTEM_THREAD(ENABLED);

void setup() {
  gpio.begin();

  gpio.pinMode(GPIO_PIN_LED, OUTPUT);
  gpio.pinMode(GPIO_PIN_BUTTON, INPUT);

  gpio.digitalWrite(GPIO_PIN_LED, HIGH); // LED off
}

void loop() {
  uint8_t buttonStatus = gpio.digitalRead(GPIO_PIN_BUTTON);

  if (buttonStatus != lastButtonStatus) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonStatus == LOW) {
      gpio.digitalWrite(GPIO_PIN_LED, LOW); // LED On
    } else {
      gpio.digitalWrite(GPIO_PIN_LED, HIGH); // LED Off
    }
  }

  lastButtonStatus = buttonStatus;
}
