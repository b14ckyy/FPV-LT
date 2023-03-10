

//*************************SETTINGS*************************
int repeats = 5;                // Number of test iterations to average result
int pauseTime = 1500;           // Time between measurements to let camera finish adjustment (in ms)
int brightnessThreshold = 10;   // threshold in mV to detect image change on screen. Raise if video noise causes false triggers
bool printRaw = true;           // Print raw measurement data of the last run over serial
bool printEveryResult = true;   // Print every measurement over Serial
bool useSevenSegDisplay = true; // Use seven segment Display outputs (Only Arduino Mega)
bool useOledDisplay = true;    // Enable SSD1306 Display output. Do not enable if useSevenSegDisplay is used

//************************DON'T TOUCH***********************
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SevSeg.h>
// https://github.com/DeanIsMe/SevSeg

unsigned int Uptime;
unsigned int Timestamps[1000];
unsigned int ScreenBright[1000];
float latencyOut = 0;
float latencyAvrg = 0;
float latencyMax = 0;
float latencyMin = 1023;
bool start = 0;
bool latencySet = 0;
bool run = false;
bool oledResultWritten = false;
unsigned long displayTime = 0;
int baseline = 0;
SevSeg sevseg;

#define sensor A0
#define LEDg 12
#define LEDr 13
#define LEDw 2
#define button 3
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO_HEIGHT 51
#define LOGO_WIDTH 128
#define SSD1306_NO_SPLASH

static const unsigned char PROGMEM BootLogo[] = {
    // Boot Logo. Don't forget to change LOGO_HEIGHT and LOGO_WIDTH
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e,
    0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e,
    0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff,
    0xff, 0xff, 0x50, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x05, 0x7f, 0xff,
    0xff, 0xff, 0xff, 0xfe, 0x80, 0x00, 0x07, 0xc0, 0x03, 0xe0, 0x00, 0x00, 0xbf, 0xff, 0xff, 0xff,
    0x7f, 0x7f, 0xff, 0xff, 0xff, 0x40, 0x07, 0x80, 0x01, 0xe0, 0x05, 0x7f, 0xff, 0xff, 0xfd, 0xff,
    0x7e, 0x00, 0xbf, 0xff, 0xff, 0xff, 0xe7, 0x80, 0x01, 0xef, 0xff, 0xff, 0xff, 0xfa, 0x00, 0x7e,
    0x10, 0x00, 0x00, 0x05, 0x7f, 0xff, 0xf7, 0x80, 0x01, 0xe7, 0xff, 0xff, 0x40, 0x00, 0x00, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe7, 0x80, 0x01, 0xef, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0xbf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{
  Serial.begin(115200);
  pinMode(sensor, INPUT);
  pinMode(button, INPUT);
  pinMode(LEDg, OUTPUT);
  pinMode(LEDr, OUTPUT);
  pinMode(LEDw, OUTPUT);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS) && useOledDisplay == true)
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  byte numDigits = 4;
  byte digitPins[] = {22, 23, 24, 25};
  byte segmentPins[] = {26, 27, 28, 29, 30, 31, 32, 33};
  bool resistorsOnSegments = 0;
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(10);

  if (useOledDisplay == true)
  {
    display.clearDisplay();
    display.drawBitmap((SCREEN_WIDTH - LOGO_WIDTH) / 2, (SCREEN_HEIGHT - LOGO_HEIGHT) / 2, BootLogo, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
    delay(3000);
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(16, 25);
    display.cp437(true);
    display.write("READY!");
    display.setTextSize(1);
    display.display();
  }
}

void loop()
{
  start = digitalRead(button);
  analogWrite(LEDg, 32);
  digitalWrite(LEDw, LOW);
  float latency[repeats];
  if (useSevenSegDisplay == true)
  {
    sevseg.refreshDisplay();
  }

  // Starting test procedure if Button has set "start" variable to 1
  if (start == 1)
  {
    analogWrite(LEDr, 64);
    digitalWrite(LEDg, LOW);
    latencyAvrg = 0;
    latencyOut = 0;
    latencyMax = 0;
    latencyMin = 1023;
    run = true;

    if (useOledDisplay == true)
    {
      display.clearDisplay();
    }

    display.clearDisplay();

    // Title for Print all Latency Measurements
    Serial.print("LATENCY MEASUREMENTS \n \n");
    for (int r = 0; r < repeats; r++)
    {
      latencySet = 0;
      digitalWrite(LEDw, HIGH);
      sevseg.blank();
      baseline = analogRead(sensor);
      Uptime = micros() / 100;
      // Taking 1000 measurement points
      for (int i = 0; i < 1000; i++)
      {
        Timestamps[i] = (micros() / 100 - Uptime);
        ScreenBright[i] = analogRead(sensor);
      }
      digitalWrite(LEDw, LOW);

      // Output OLED Display Coordinate system
      if (useOledDisplay == true)
      {
        display.clearDisplay();
        display.drawLine(6, 0, 6, 54, SSD1306_WHITE);
        display.drawLine(6, 54, 127, 54, SSD1306_WHITE);
        display.setCursor(0, 0);
        display.write("1");
        display.setCursor(0, 46);
        display.write("0");
        display.setCursor(110, 46);
        display.write("100");
        display.setCursor(0, 56);
        display.write("Latency: ");
      }

      // Check for brightness flank timestamp
      for (int j = 0; j < 1000; j++)
      {
        if ((baseline + brightnessThreshold) <= ScreenBright[j] && latencySet == 0)
        {
          latency[r] = Timestamps[j];
          latencySet = 1;
          // Printing all Latency measurements via serial
          Serial.print("Masurement ");
          Serial.print(r + 1);
          Serial.print(": ");
          Serial.print(latency[r] / 10);
          Serial.print("ms\n");
          if (useOledDisplay == true)
          {
            // Write Latency on OLED
            display.print(latency[r] / 10);
            display.display();
          }
        }
      }
      // Draw Graph
      if (useOledDisplay == true)
      {
        for (int j = 0; j < 1000; j += 5)
        {
          display.drawPixel(((Timestamps[j] / 8) + 6), (((int)ScreenBright[j] / -14) + 54), SSD1306_WHITE);
        }
        display.display();
      }

      // Time delay for camera to readjust after each measurement and show current measurement on displays
      for (uint32_t tStart = millis(); (millis() - tStart) < pauseTime; useSevenSegDisplay == true)
      {
        if (useSevenSegDisplay == true)
        {
          sevseg.setNumber(latency[r], 1);
          sevseg.refreshDisplay();
        }
      }
    }

    for (int i = 0; i < repeats; i++)
    {
      // Add up latency values for average calculations
      latencyAvrg = (latencyAvrg + latency[i]);

      // detect and store min and max values
      if (latencyMax < latency[i])
      {
        latencyMax = latency[i];
      }
      if (latencyMin > latency[i])
      {
        latencyMin = latency[i];
      }
    }

    // calculate results
    latencyAvrg = latencyAvrg / ((float)repeats * 10.0f);
    latencyMax = latencyMax / 10.0f;
    latencyMin = latencyMin / 10.0f;

    Serial.print("\n######################## \n\nRESULTS: \n\n");
    Serial.print("Average Latency: ");
    Serial.print(latencyAvrg);
    Serial.print("ms \n");
    Serial.print("Max Latency:     ");
    Serial.print(latencyMax);
    Serial.print("ms \n");
    Serial.print("Min Latency:     ");
    Serial.print(latencyMin);
    Serial.print("ms \n");

    if (printRaw == true)
    {
      // Print Timestamp and Data Arrays over Serial during last run
      Serial.print("\n######################## \n \nLATENCY RAW DATA OF LAST MEASUREMENT\n \n");
      for (int i = 0; i < 1000; i++)
      {
        latencyOut = (float)Timestamps[i] / 10.0f;
        Serial.print(latencyOut);
        Serial.print(",");
        Serial.print(ScreenBright[i]);
        Serial.print("\n");
      }
    }

    baseline = 0;
    digitalWrite(LEDr, LOW);
    displayTime = millis();
    start = 0;
    display.display();
  }

  if (run == true)
  {

    if ((millis() - displayTime) <= 2000 && latencyMin < 1023 && useSevenSegDisplay == true)
    {
      sevseg.setNumberF(latencyMin, 1);
      sevseg.setSegmentsDigit(0, 8);
    }
    if ((millis() - displayTime) > 2000 && (millis() - (displayTime)) <= 4000 && useSevenSegDisplay == true)
    {
      sevseg.setNumberF(latencyAvrg, 1);
      sevseg.setSegmentsDigit(0, 64);
    }
    if ((millis() - displayTime) > 4000 && (millis() - (displayTime)) <= 6000 && useSevenSegDisplay == true)
    {
      sevseg.setNumberF(latencyMax, 1);
      sevseg.setSegmentsDigit(0, 1);
    }
    if ((millis() - displayTime) > 6000)
    {
      displayTime = millis();
    }

    if (useOledDisplay == true)
    {
      if ((millis() - displayTime) <= 3000 && useOledDisplay == true && oledResultWritten == false)
      {
        // Output Results on OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(2);
        display.println("RESULTS:");
        display.setTextSize(1);
        display.drawLine(0, 17, 127, 17, 1);
        display.setCursor(0, 21);
        display.print("Avrg Latency: ");
        display.println(latencyAvrg);
        display.print("\nMax Latency:  ");
        display.println(latencyMax);
        display.print("\nMin Latency:  ");
        display.println(latencyMin);
        display.display();
        oledResultWritten = true;
      }
      if ((millis() - displayTime) > 3000 && useOledDisplay == true && oledResultWritten == true)
      {
        // Output OLED Display Graph
        display.clearDisplay();
        display.drawLine(6, 0, 6, 54, SSD1306_WHITE);
        display.drawLine(6, 54, 127, 54, SSD1306_WHITE);
        display.setCursor(0, 0);
        display.write("1");
        display.setCursor(0, 46);
        display.write("0");
        display.setCursor(110, 46);
        display.write("100");
        display.setCursor(0, 56);
        display.write("Average Latency: ");
        display.print(latencyAvrg / 10);
        // Draw High Res Graph
        for (int j = 0; j < 1000; j += 5)
        {
          display.drawPixel(((Timestamps[j] / 8) + 6), (((int)ScreenBright[j] / -14) + 54), SSD1306_WHITE);
        }
        display.display();
        oledResultWritten = false;
      }
    }
  }

  if (run == false && useSevenSegDisplay == true)
  {
    sevseg.setNumberF(0000, 1);
  }
}
