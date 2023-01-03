# FPV-LT
FPV Latency Tester - Fully automated Glass-to-Glass Latency Test

This tool does exactly what it's name suggests. It tests the Latency of FPV systems, simply by pressing a button. It also delivers some basic statistics and provides data for further analysis over Serial to your PC or Tablet. It can Display Data over a cheap 7-Segment 4-Digit LED Display or a SSD1306 OLED Display. 

# WHY? - Because! 
Actually this just started as a funny idea and practice Project. I just started to learn programming again (15 years after I did some rudimentary C Stuff in school) and this was my second personal training project with an actual practical use. After two long evenings it turned out to work so well, that I decided to make it a full blown project and also publish it as soon as all main functionality is there. There is still some work to do to optimize it but that will be done later. 

There are 2 other methods to test the latency. Many people use a GoPro or Phone and record video at high framerate. But because of shutter times not being in sync between camera, display and high FPS cam, the actual possible measurement precision is within a 8ms range for a 240fps camera and 2ms with one of the rare 960fps high speed cameras. To get enough data for proper miniumum, maximum and average latency (due to Screen refresh and FPV Camera Shutter times messing with the measurement starting point) it would also need multiple measurements, that take a lot of work. 

The most precise solution would be with an oscilloscope but they are expensive, not everyone can use them and to reduce the work for a big number of measurements, it would also need an even more expensive device with logging features. 

With the FPV-LT we can get close to the precision of an oscilloscope (effectively recording 6600fps) for a hardware price of less than 30 bucks. 

# WHAT? 
The Latency Tester just uses a white (or any other color, but white works best) LED, mounted to the front of a FPV camera with an light-tight adapter. On the goggle side a Photo-Resistor is mounted with a voltage devider and placed on the goggle lenses or on the FPV screen with a black TPU hood to avoid light leakage. Now the fun part begins!

Although the ATmega2560 is relatively powerful for an arduino processor, it is too slow to do a realtime sensor analysis at the needed rate to get proper results. So instead, we make use of the big SRAM and collect a bunch of data first and do the analysis later. That's why I highly recommend the 2560 CPU and not an ATmega328 (Arduino UNO).

For one measurement cycle the white LED will turn on and the Arduino is reading the Voltage from the Photoresistor 1000x and stores the value together with the current absolute timestamp based on the clock timer. This happens in 152.2ms and gives us roughly 0.15ms resolution for each reading. 
After all the datapoints are collected and stored, we then do some math magic to find the flank, at what point the voltage measurement starts to rise (10mV by default, so a very small change, that's why light leaks need to be avoided) and store the timestamp of that measurement point as the latency value. This gives us the glass to glass latency for the first screen light to lighten up.

This process is repeated 100x by default, storing 100 latency values (raw measurement data are overwritten, at the end we only have 8k of RAM and one raw dataset needs already half of that). With all 100 values stored we can then calculate the average latency as well as the maximum and minimum values.

All latency measurements and the final result is sent over serial (baud 115200) to a connected device with Serial Monitor. If needed, also the last raw dataset can be sent over serial if further analysis is needed. To use the FPV-LT standalone, you can either connect a 7-Segment 4-Digit LED Display or a SSD1306 OLED Display (or even both) to see every measurement and final results. On trhe OLED Display you can also see the measurement as a graphical output to detect discrepancies (fluctuations, measurement errors, weak measurements, etc.)

# HOW? 
Complete Documentation with Schematics will follow as soon as I got my Shield PCB and hard wired the electronics.

# NEXT?
I have some feature Ideas still pending that should not need any hardware change. To be sure, leave some room for 2 more buttons, left and right of the START Button. 
- Refactor code for better editing in the future (split code into multiple files and functions)
- Check possibility to use an Arduino UNO with reduced resolution
- Add ESP32 support (needs to limit data collection loop speed with a timer)
- Add option to change measurement settings with on-display menu and use EEPROM to store them
