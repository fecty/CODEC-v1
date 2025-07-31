// Included Libraries
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// Pin Definitions
#define SENSOR_PIN A3 // MEMS Carbon Monoxide Sensor
#define OLED_SDA A4   // I2C Serial Data for OLED Display
#define OLED_SCL A5   // I2C Serial Clock for OLED Display
#define BUZZER_PIN 9  // Buzzer +ve Terminal
#define SW_PIN 8      // Rotary Encoder Button
#define DT_PIN 7      // Rotary Encoder Channel A
#define CLK_PIN 6     // Rotary Encoder channel B
#define LED_PIN 3     // LED Data pin

// NeoPixel LED Definitions
#define NUM_LEDS 24    // Number of LEDs in the strip
#define BRIGHTNESS 255 // LED brightness

// Definitations
#define BAUD_RATE 115200  // Serial communication baud rate
#define DELAY_MS 200      // Delay after each iteration of loop in milliseconds.
#define SCREEN_WIDTH 128  // OLED Display Width
#define SCREEN_HEIGHT 64  // OLED Display Height
#define OLED_ADDRESS 0x3C // I2C Address for OLED Display
#define OLED_TEXT_SIZE 2  // OLED Text Size
#define OLED_TEXT_COLOR WHITE
#define SENSOR_REFRESH_RATE 500 // Sensor refresh rate
#define BUZZER_THRESHOLD 10.0   // Threshold for buzzer to activate

// Other Definitions
#define OLED_START_X 0
#define OLED_START_Y 0
#define OLED_END_X 128
#define OLED_END_Y 64
#define OLED_HEAD_HEIGHT 16

#define FACTOR 2

// Variable Declarations
int sensorValue; // CO Sensor Value 0-1023
double CO_PPM, CO_PPZ, test_PPZ;
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB);

// OLED Display Declaration
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool testMode = false;

// Variables for managing timing
unsigned long previousMillis = 0; // Stores the last time the buzzer toggled
bool buzzerState = false;         // Tracks the current state of the buzzer
int repeatCount = 0;              // Tracks how many times the pattern has repeated
int totalRepeats = 0;             // Total repeats for the current tune
int durationOn = 0;               // On duration for the buzzer
int durationOff = 0;              // Off duration for the buzzer

// Graph parameters
const int graphTop = OLED_HEAD_HEIGHT + 1;
const int graphBottom = OLED_END_Y - 2;
const int graphLeft = OLED_START_X + 1;
const int graphRight = OLED_END_X - 1;
int testCounter = 0;

// Function Declarations
double calc_ppm(int x);                                             // x is analog voltage taken from sensor
double calc_ppz(int x);                                             // custom unit from 0 to 100
bool run_after(int time_ms, int counter);                           // Checks if a certain time has passed
void drawBorders();                                                 // Draws borders on the OLED display
uint32_t getColorFromValue(double value, Adafruit_NeoPixel &strip); // Gets color based on CO value
void playWarningTune(int value);                                    // Plays warning tune
void playWarningTuneNonBlocking(int value);                         // Plays warning tune without blocking
void plotValueOnGraph(int value);                                   // Plots value on the graph

// Setup Function
void setup()
{
  Serial.begin(BAUD_RATE);

  // Initialize OLED display
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

  // Pin Mode Setup
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(DT_PIN, INPUT);
  pinMode(CLK_PIN, INPUT);

  // Initialize NeoPixel strip
  strip.begin();
  strip.setBrightness(100);

  // Initialize Display
  display.clearDisplay();
  display.setTextSize(OLED_TEXT_SIZE);
  display.setTextColor(OLED_TEXT_COLOR);
  display.setCursor(0, 0);
  display.setTextWrap(false);

  Serial.println("Carbon Monoxide Detector/Decorative Night Light\nZurain Nazir | E: nazirzurain@gmail.com | P: +91 7006775359\nDec. 2024");
}

// Main Loop
void loop()
{
  // Check if the button is pressed to toggle test mode
  if (digitalRead(SW_PIN) == 0)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    testMode = !testMode;
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // LOOP START CODE
  display.fillRect(OLED_START_X, OLED_START_Y, 128, 16, BLACK);
  strip.clear();
  display.setCursor(20, 1);
  digitalWrite(BUZZER_PIN, LOW);
  drawBorders();

  // Reset test counter
  if (testCounter > 100)
  {
    testCounter = 0;
  }

  // Read sensor value and calculate CO concentration
  sensorValue = analogRead(SENSOR_PIN);
  CO_PPM = calc_ppm(sensorValue);
  CO_PPZ = calc_ppz(sensorValue);
  test_PPZ = sin(millis() / 400) * 50 + 50;

  // LOOP MAIN CODE
  if (testMode)
  {
    // Test mode operations
    if (test_PPZ > BUZZER_THRESHOLD)
    {
      playWarningTuneNonBlocking(test_PPZ);
    }
    plotValueOnGraph(test_PPZ / 1.2 + 10);
    strip.fill(getColorFromValue(test_PPZ, strip));

    display.print(test_PPZ);
    display.print("PPT");
    Serial.print("CO Concentration in PPT:");
    Serial.println(test_PPZ);
  }
  else
  {
    // Normal mode operations
    if (CO_PPZ > BUZZER_THRESHOLD)
    {
      playWarningTuneNonBlocking(CO_PPZ);
    }
    plotValueOnGraph(CO_PPZ);
    strip.fill(getColorFromValue(CO_PPZ, strip));
    display.print(CO_PPZ);
    display.print("PPZ");

    Serial.print("CO Concentration in PPZ:");
    Serial.println(CO_PPZ);
  }

  strip.show();

  // LOOP END CODE
  display.display();
  delay(DELAY_MS);
  testCounter++;
}

// Function to calculate CO concentration in PPM
double calc_ppm(int x) // High possibility of Inaccuracy
{
  return 5.03 * x - 145.91;
}

// Function to calculate CO concentration in PPZ (custom unit)
double calc_ppz(int x) // Custom Unit for CO concentration
{
  return (double)x * 100 / 1023;
}

// Function to draw borders on the OLED display
void drawBorders()
{
  display.drawRect(OLED_START_X, OLED_START_Y, OLED_END_X, OLED_HEAD_HEIGHT, WHITE);                    // header rectangle
  display.drawRect(OLED_START_X, OLED_HEAD_HEIGHT, OLED_END_X, OLED_END_Y - (OLED_HEAD_HEIGHT), WHITE); // graph rectangle
}

// Function to get color from CO value for NeoPixel
uint32_t getColorFromValue(double value, Adafruit_NeoPixel &strip)
{
  // Clamp the value between 0.0 and 100.0
  if (value < 0.0)
    value = 0.0;
  if (value > 100.0)
    value = 100.0;

  uint8_t r, g, b;

  if (value <= 50.0)
  {
    // Green to Yellow (value 0 to 50)
    r = (uint8_t)(value / 50.0 * 255); // Gradually increase red
    g = 255;                           // Full green
    b = 0;                             // No blue
  }
  else
  {
    // Yellow to Red (value 50 to 100)
    r = 255;                                     // Full red
    g = (uint8_t)((100.0 - value) / 50.0 * 255); // Gradually decrease green
    b = 0;                                       // No blue
  }

  // Return the color in NeoPixel format
  return strip.Color(r, g, b);
}

// Function to play warning tune
void playWarningTune(int value)
{
  // Clamp the value between 20 and 100
  if (value < 20)
    value = 20;
  if (value > 100)
    value = 100;

  // Map the value to intensity parameters
  int durationOn = map(value, 20, 100, 200, 50);  // On duration decreases as value increases
  int durationOff = map(value, 20, 100, 200, 30); // Off duration decreases as value increases
  int repeatCount = map(value, 20, 100, 3, 10);   // Repeat count increases as value increases

  // Play the warning tune based on intensity
  for (int i = 0; i < repeatCount; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH); // Turn buzzer ON
    delay(durationOn);
    digitalWrite(BUZZER_PIN, LOW); // Turn buzzer OFF
    delay(durationOff);
  }
}

// Non-blocking function to play warning tune
void playWarningTuneNonBlocking(int value)
{
  static int currentStage = 0;

  // Clamp the value between 20 and 100
  if (value < 20)
    value = 20;
  if (value > 100)
    value = 100;

  // Calculate parameters for the warning tune
  durationOn = map(value, 20, 100, 200, 50);  // On duration decreases as value increases
  durationOff = map(value, 20, 100, 200, 30); // Off duration decreases as value increases
  totalRepeats = map(value, 20, 100, 3, 10);  // Repeat count increases as value increases

  unsigned long currentMillis = millis();

  // Handle buzzer timing
  if (buzzerState && (currentMillis - previousMillis >= durationOn))
  {
    digitalWrite(BUZZER_PIN, LOW); // Turn buzzer OFF
    buzzerState = false;
    previousMillis = currentMillis;
    repeatCount++; // Increment the pattern repeat count
  }
  else if (!buzzerState && (currentMillis - previousMillis >= durationOff))
  {
    if (repeatCount < totalRepeats)
    {
      digitalWrite(BUZZER_PIN, HIGH); // Turn buzzer ON
      buzzerState = true;
      previousMillis = currentMillis;
    }
    else
    {
      // Reset repeat count to stop the tune after the desired repeats
      repeatCount = 0;
    }
  }
}

// Function to plot value on the graph
void plotValueOnGraph(int value)
{
  static int lastX = graphLeft;   // Keeps track of the last x-coordinate
  static int lastY = graphBottom; // Keeps track of the last y-coordinate

  // Constrain the value to 0-100
  value = constrain(value, 0, 100);

  // Map the value to the graph's vertical range
  int y = map(value, 0, 100, graphBottom, graphTop);

  // Increment the x-coordinate
  int x = lastX + 1;
  if (x >= graphRight)
  {
    // Clear the graph area if x exceeds the boundary and reset
    display.fillRect(graphLeft + 1, graphTop + 1, graphRight - graphLeft - 2, graphBottom - graphTop - 2, SSD1306_BLACK);
    // display.clearDisplay();
    x = graphLeft + 1;
    y = graphBottom + 1;
    lastX = x;
    lastY = y;
  }

  // Draw a line connecting the last point to the new point
  display.drawLine(lastX, lastY, x, y, SSD1306_WHITE);

  // Update the last point
  lastX = x;
  lastY = y;

  // Display the updated graph
  display.display();
}