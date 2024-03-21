#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <PowderFeederOLED.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR 0x3C

#define ENC_A_PIN 5
#define ENC_B_PIN 4
#define ENC_SW_PIN 14

#define RED_LED_PIN 32
#define GRN_LED_PIN 33 
#define BLU_LED_PIN 25

volatile bool NEW_ENCODER_INFO = false;
volatile bool FEEDER_SPINNING = false;
volatile int ENCODER_VALUE;
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
    
    if((LAST_TIME - millis()) < DEBOUNCE_TIME_MS)
    {
      return;
    }
    
    byte a_status = digitalRead(ENC_A_PIN);
    if(a_status != 1)
    {
      return;
    }
    
    if(a_status == digitalRead(ENC_B_PIN))
    {
      ENCODER_VALUE += 1;
    } else {
      ENCODER_VALUE -= 1;
    }
    
    if(ENCODER_VALUE < 0) ENCODER_VALUE = 0;
    LAST_TIME = millis();
    NEW_ENCODER_INFO = true;
   
}


void setup() {
  Serial.begin(115200);
  LAST_TIME = 0;
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
    encoderSWISR, CHANGE);
// LCD test    
for(int i = 0 ; i<3; i++)
{
  PF_DISPLAY.flashInvertLCD();
  delay(100);
}

}

void loop() {
  PF_DISPLAY.drawRPMScreen(0.0);
  if(NEW_ENCODER_INFO)
   {
    Serial.print(" ENCVAL: ");
    Serial.print(ENCODER_VALUE);
    Serial.print("\n");
    NEW_ENCODER_INFO = false;
   }
}