# Carbon Monoxide Detector Project

This project implements a carbon monoxide detector using an Arduino Nano, a MEMS CO sensor, an OLED display, a NeoPixel LED strip, and a buzzer. The device measures CO concentration, displays it on the OLED screen, indicates the level with the NeoPixel LEDs, and triggers a buzzer alarm when the concentration exceeds a predefined threshold.

## Project Structure

The project is structured as follows:

- `.pio`: PlatformIO build environment and related files.
- `.vscode`: VS Code configuration files.
- `include`: Contains project-specific header files (currently empty, but intended for future expansion).
- `lib`: Contains project-specific libraries (currently empty, but can be used for custom library code).
- `src`: Contains the main source code (`main.cpp`).
- `test`: Contains unit tests for the project (currently empty).
- `platformio.ini`: PlatformIO project configuration file.
- `README.md`: This file, providing an overview of the project.
- `.gitignore`: Specifies intentionally untracked files that Git should ignore.

### Source Code (`src/main.cpp`)

The `src/main.cpp` file contains the main application logic. It includes the following sections:

- **Included Libraries**: Includes necessary libraries such as Arduino.h, Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h, and Adafruit_NeoPixel.h.
- **Pin Definitions**: Defines the pins used for various components like the CO sensor, OLED display, buzzer, rotary encoder, and NeoPixel LED strip.
- **Definitions**: Defines constants such as baud rate, delays, screen dimensions, sensor refresh rate, and buzzer threshold.
- **Variable Declarations**: Declares global variables for sensor values, CO concentration, OLED display, NeoPixel strip, and buzzer control.
- **Function Declarations**: Declares functions used in the project, such as `calc_ppm`, `calc_ppz`, `drawBorders`, `getColorFromValue`, `playWarningTune`, `playWarningTuneNonBlocking`, and `plotValueOnGraph`.
- **Setup Function**: Initializes the serial communication, OLED display, pin modes, and NeoPixel strip.
- **Loop Function**: Reads sensor values, calculates CO concentration, displays the concentration on the OLED screen, controls the NeoPixel LEDs based on the concentration, and triggers the buzzer if the concentration exceeds the threshold.
- **Function Definitions**: Implements the functions declared earlier, including CO concentration calculation, OLED display drawing, NeoPixel color mapping, buzzer control, and graph plotting.

### PlatformIO Configuration (`platformio.ini`)

The `platformio.ini` file configures the PlatformIO build environment, including the platform, board, framework, monitor speed, and library dependencies.

```ini
[env:nanoatmega328]
platform = atmelavr
board = nanoatmega328new
framework = arduino
monitor_speed = 115200
lib_deps =
	adafruit/Adafruit GFX Library@^1.11.11
	adafruit/Adafruit SSD1306@^2.5.13
	adafruit/Adafruit NeoPixel@^1.12.3
```

## Libraries

The project uses the following libraries:

- **Adafruit GFX Library**: Provides graphics functions for the OLED display.
- **Adafruit SSD1306**: Provides functions for controlling the OLED display.
- **Adafruit NeoPixel**: Provides functions for controlling the NeoPixel LED strip.

## Functionality

The carbon monoxide detector performs the following functions:

- Measures CO concentration using a MEMS CO sensor.
- Displays the CO concentration on the OLED screen in PPZ (custom unit) and optionally in PPM.
- Indicates the CO concentration level using a NeoPixel LED strip, with colors ranging from green (low concentration) to red (high concentration).
- Triggers a buzzer alarm when the CO concentration exceeds a predefined threshold.
- Includes a test mode that simulates CO concentration for testing purposes.
- Plots the CO concentration on a graph on the OLED screen.

## Pinout

- `SENSOR_PIN (A3)`: Analog input from the MEMS CO sensor.
- `OLED_SDA (A4)`: I2C SDA pin for the OLED display.
- `OLED_SCL (A5)`: I2C SCL pin for the OLED display.
- `BUZZER_PIN (9)`: Digital output pin for the buzzer.
- `SW_PIN (8)`: Digital input pin for the rotary encoder button (used to toggle test mode).
- `DT_PIN (7)`: Digital input pin for the rotary encoder Channel A.
- `CLK_PIN (6)`: Digital input pin for the rotary encoder Channel B.
- `LED_PIN (3)`: Digital output pin for the NeoPixel LED strip.

## Future Enhancements

- Implement data logging to record CO concentration over time.
- Add a user interface for setting the buzzer threshold and other parameters.
- Incorporate a real-time clock (RTC) module for accurate timekeeping.
- Develop a mobile app for remote monitoring and control.
