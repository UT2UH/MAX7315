/* Example usage for MAX7315 library.
 * Demonstrates the use of interrupt functionality of the MAX7315.
 *
 * You need to connect:
 * LED to MAX7315 IO pin 4
 * Button to MAX7315 IO pin 0
 * MAX7315 INT pin to D4 pin
 */
#include <Wire.h>
#include "MAX7315.h"

#define GPIO_PIN_LED           4  // LED connected to GPIO-0 as output
#define GPIO_PIN_BUTTON        0  // Button connected to GPIO-1 as input
#define PIN_INTERRUPT          4  // Interrupt pin

MAX7315 gpio;
volatile uint8_t ioState = LOW;
// Last time the output pin was toggled
unsigned long lastDebounceTime = 0;
// Debounce time; increase if the output flickers
unsigned long debounceDelay = 25;

void io() {
  ioState = HIGH;
  // Capture all the flickering
  lastDebounceTime = millis();
}


void setup() {
  Serial.begin(115200);
  gpio.begin();

  Wire.begin();
  Wire.setClock(400000);

  gpio.pinMode(GPIO_PIN_LED, OUTPUT);
  gpio.pinMode(GPIO_PIN_BUTTON, INPUT);

  gpio.digitalWrite(GPIO_PIN_LED, HIGH); // LED off

  attachInterrupt(PIN_INTERRUPT, io, FALLING);
}

void loop() {
  if (ioState == HIGH) {
    if (millis() - lastDebounceTime > debounceDelay) {
      uint8_t buttonStatus = gpio.digitalRead(GPIO_PIN_BUTTON);
      if (buttonStatus == LOW) {
        gpio.digitalWrite(GPIO_PIN_LED, LOW); // LED On
      } else {
        gpio.digitalWrite(GPIO_PIN_LED, HIGH); // LED Off
      }
      ioState = LOW; // Reset the state for another interrupt
    }
  }
}
