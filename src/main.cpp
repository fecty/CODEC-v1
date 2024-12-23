// Included Libraries
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin Definitions
#define SENSOR_PIN A3 // MEMS Carbon Monoxide Sensor
#define OLED_SDA A4   // I2C Serial Data for OLED Display
#define OLED_SCL A5   // I2C Serial Clock for OLED Display
#define BUZZER_PIN 9  // Buzzer +ve Terminal
#define SW_PIN 8      // Rotary Encoder Button
#define DT_PIN 7      // Rotary Encoder Channel A
#define CLK_PIN 6     // Rotary Encoder channel B

// Definitations
#define BAUD_RATE 115200
#define DELAY_MS 100      // Delay after each itteration of loop in miliseconds.
#define SCREEN_WIDTH 128  // OLED Display Width
#define SCREEN_HEIGHT 64  // OLED Display Height
#define OLED_ADDRESS 0x3C // I2C Address for OLED Display
#define OLED_TEXT_SIZE 2
#define OLED_TEXT_COLOR WHITE

// Other Definitions
#define OLED_START_X 0
#define OLED_START_Y 0
#define OLED_END_X 128
#define OLED_END_Y 64
#define OLED_HEAD_HEIGHT 16

#define FACTOR 2

// Variable Declarations
int sensorValue; // CO Sensor Value 0-1023
double CO_PPM, CO_PPZ;
int loopCounter;
// OLED Display Declaration
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

double calc_ppm(int x); // x is analog voltage taken from sensor
double calc_ppz(int x); // custom unit from 0 to 100
bool run_after(int time_ms, int counter);
void setup()
{
  Serial.begin(BAUD_RATE);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    while (1)
    {
      Serial.println("Restart board...");
      delay(2000);
    }
  }
  delay(2000); // Wait for Display to initialize

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SW_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(CLK_PIN, INPUT);

  display.clearDisplay();
  display.setTextSize(OLED_TEXT_SIZE);
  display.setTextColor(OLED_TEXT_COLOR);
  display.setCursor(0, 0);
  display.setTextWrap(false);
  // display.invertDisplay(true);
  // display.setRotation(2);

  Serial.println("Carbon Monoxide Detector/Decorative Night Light\nZurain Nazir | E: nazirzurain@gmail.com | P: +91 7006775359\nDec. 2024");
}

void loop()
{
  // LOOP START CODE
  display.clearDisplay();
  display.setCursor(10, 1);

  // LOOP MAIN CODE
  sensorValue = analogRead(SENSOR_PIN);
  CO_PPM = calc_ppm(sensorValue);
  CO_PPZ = calc_ppz(sensorValue);

  display.print(CO_PPZ);
  display.print("PPZ");

  Serial.print("CO Concentration in PPZ:");
  Serial.println(CO_PPZ);
  display.drawRect(OLED_START_X, OLED_START_Y, OLED_END_X, OLED_HEAD_HEIGHT, WHITE); // header rectangle
  display.drawRect(OLED_START_X, OLED_HEAD_HEIGHT, OLED_END_X, OLED_END_Y - (OLED_HEAD_HEIGHT), WHITE);

  if (run_after(10000, loopCounter))
  {
    // DELAY_MS * 100 = 10Sec
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
    delay(1000);
  }

  // LOOP END CODE
  display.display();
  delay(DELAY_MS);
  loopCounter++;
}

double calc_ppm(int x) // High possibility of Inaccuracy
{
  return 5.03 * x - 145.91;
}
double calc_ppz(int x) // Custom Unit for CO concentration
{
  return (double)x * 100 / 1023;
}

bool run_after(int time_ms, int counter)
{
  // time taken = DELAY_MS * PortionFactor -> PF = TimeTaken/DELAY_MS

  return !(counter % (time_ms / DELAY_MS));
}