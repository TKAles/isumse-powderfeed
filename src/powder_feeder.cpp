#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <PowderFeederOLED.h>

// Hookup and I2C macros for the OLED screen
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR 0x3C

// Pin macros for the rotary switch
#define ENC_A_PIN 5
#define ENC_B_PIN 4
#define ENC_SW_PIN 14

// Pin macros for blinkenlights.
#define RED_LED_PIN 32
#define GRN_LED_PIN 33 
#define BLU_LED_PIN 25

// Configuration settings, most should be pretty explainatory
const int ENCODER_RPM_RESOLUTION_STEP = 2;
const int ENCODER_MIN_RPM_CLIP = 0;
const int ENCODER_MAX_RPM_CLIP = 300;

// Global status flags and values used by the ISRs
volatile bool NEW_ENCODER_INFO = false;
volatile bool FEEDER_SPINNING = false;
volatile int ENCODER_VALUE;
volatile bool FLASH_EVENT;
volatile unsigned long LAST_TIME;
const unsigned long DEBOUNCE_TIME_MS = 200;
PowderFeederOLED PF_DISPLAY;

// Interrupt for the encoder switch
// the IRAM_ATTR instructs the ESP to keep this
// in its instruction RAM instead of the flash
IRAM_ATTR void encoderSWISR()
{
  if(FEEDER_SPINNING)
  {
    FEEDER_SPINNING = false;
    digitalWrite(GRN_LED_PIN, 0);
    digitalWrite(RED_LED_PIN, 1);
  }
  else
  {
    FEEDER_SPINNING = true;
    digitalWrite(GRN_LED_PIN, 1);
    digitalWrite(RED_LED_PIN, 0);
  }
}
// Interrupt for the encoder rotation
IRAM_ATTR void encoderISRA()
{
    // Has enough time passed between two sucessive ISR firings?
    // if not, do nothing
    if((LAST_TIME - millis()) < DEBOUNCE_TIME_MS)
    {
      return;
    }
    // Is the A leg of the encoder high? If not do nothing.
    byte a_status = digitalRead(ENC_A_PIN);
    if(a_status != 1)
    {
      return;
    }
    // If the A leg is high, and the B leg is also high, we must be
    // going clockwise, otherwise we must be going CCW.
    // Update the numerical value with one 'tick' of resolution and
    // set a new last fired timestamp, notify the main loop that there
    // is also new information for the display.
    if(a_status == digitalRead(ENC_B_PIN))
    {
      ENCODER_VALUE += ENCODER_RPM_RESOLUTION_STEP;
    } else {
      ENCODER_VALUE -= ENCODER_RPM_RESOLUTION_STEP;
    }
    LAST_TIME = millis();
    NEW_ENCODER_INFO = true;
}


void setup() {
  Serial.begin(115200);
  LAST_TIME = 0;
  ENCODER_VALUE = 0;
  PF_DISPLAY.setupLCD(OLED_WIDTH, OLED_HEIGHT, OLED_ADDR);
   
  // Set the modes for various inputs/outputs
  pinMode(ENC_A_PIN, INPUT);
  pinMode(ENC_B_PIN, INPUT);
  pinMode(ENC_SW_PIN, INPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GRN_LED_PIN, OUTPUT);
  pinMode(BLU_LED_PIN, OUTPUT);
  
  // Startup Self-Test
  digitalWrite(RED_LED_PIN, 0);
  digitalWrite(GRN_LED_PIN, 0);
  digitalWrite(BLU_LED_PIN, 0);
  delay(500);

  // Attach the interrupts for the encoder and switch
  attachInterrupt(
    digitalPinToInterrupt(ENC_A_PIN),
    encoderISRA, HIGH);
  attachInterrupt(
    digitalPinToInterrupt(ENC_SW_PIN),
    encoderSWISR, LOW);
// LCD test    
for(int i = 0 ; i<3; i++)
{
  PF_DISPLAY.flashInvertLCD();
  delay(100);
}

}

void loop() {
  // Check the encoder value is within configured limits,
  // otherwise clip it.
  if(ENCODER_VALUE < ENCODER_MIN_RPM_CLIP)
  {
    ENCODER_VALUE = ENCODER_MIN_RPM_CLIP;
    PF_DISPLAY.minimumAlertFlash();
  }
  if(ENCODER_VALUE > ENCODER_MAX_RPM_CLIP)
  {
    ENCODER_VALUE = ENCODER_MAX_RPM_CLIP;
    FLASH_EVENT = true;
  }

  // See if new encoder information should be printed to serial
  if(NEW_ENCODER_INFO)
   {
    Serial.print(" ENCVAL: ");
    Serial.print(ENCODER_VALUE);
    Serial.print("\n");
    NEW_ENCODER_INFO = false;
   }
  // Draw the current RPM value
  // value is scaled by ten and stored as an integer
  // for speed / solve interrupt crashes.
  // e.g. 118 actually means 11.8
  PF_DISPLAY.drawRPMScreen(ENCODER_VALUE);
  if(FLASH_EVENT)
  {
    PF_DISPLAY.flashInvertLCD();
    FLASH_EVENT = false;
  }

}