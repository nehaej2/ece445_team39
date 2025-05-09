3/6/25 
Motor Power System Info: 
 
For 12V to 5V converter: LM2596-5.0 step-down (buck) converter 
 
LM2596-5.0 Data Sheet: 
https://www.ti.com/lit/ds/symlink/lm2596.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=
1741287428008 
 
Max input voltage: 40V 
Max current limit: 6.9A (will need to use resistors to step down the current flow before entering 
regulator) 
Max output current: ~1.4A 
 
 
Motor subsystem current requirements: 
● Motor driver — 0-40mA 
● ESP32 — 352mA 
● DWM1000 — 160mA  
 
Battery: 
KBT 12V 5200mAh 
Continuous discharge current: 5.2A (1C) 
Max continuous discharge rate: 15.6A (3C) 
https://www.amazon.com/dp/B0C242DYT1?ref=emc_s_m_5_i_atc&th=1 
 
AA Batteries: 
Voltage: 1.5A 
Service hours at 500 mA of usage: roughly 3 hours 
https://www.duracell.com/wp-content/uploads/2016/03/OP15US0122-A.pdf

---

3/11/25 
Breadboard demo  
Motor driver breakout board: 
https://www.digikey.com/en/products/detail/adafruit-industries-llc/2448/5353672  
Motor driver TB6612FNG: 
https://cdn-shop.adafruit.com/datasheets/TB6612FNG_datasheet_en_20121101.pdf  
Programming TB6612FNG driver w Arduino via ESP32: 
https://www.instructables.com/Simple-2-Wheel-ESP32-Robot-Using-the-TB6612FNG-Dua/  
 
 
ESP32-S3-DevKitC-1 Info (our ESP32 Dev Board): 
Getting started: 
https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/user_guid
e.html#getting-started 
 
 
 
ARDUINO SET-UP: 
https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html 
- 
Paste stable link in Board Manager URLs under preferences 
- 
Download esp32 library under board manager, select ESP32S3 Dev Module, THEN 
RESTART ARDUINO IDE 
- 
Select right COM port. Use windows ‘device manager’ – find USB to UART manager, 
that is the ESP32 
 
Motor Control (software) Info: 
 
Library with Motor Control for TB6612FNG – currently not working with our set up, but can 
reference for ideas:  
https://github.com/pablopeza/TB6612FNG_ESP32/tree/master 
 
ALL official libraries and APIs supported by Arduino IDE for ESP32: 
https://docs.espressif.com/projects/arduino-esp32/en/latest/libraries.html#apis 
 
LED Control (PWM signals we’ll be using for motor control) references: 
https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ledc.html 
 
pinMode() reference (intializing pins): 
https://docs.arduino.cc/language-reference/en/functions/digital-io/pinMode/ 
 
Examples of Arduino ESP32 API usage: 
https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples

---

3/30/25 
Motor Driver Comparisons: 
 
Resources: 
https://www.hackster.io/news/tb6612fng-motor-driver-better-than-the-l298n-7499a7574e63#:~:te
xt=The%20TB6612FNG%20is%20smaller%20than,mode%20that%20the%20L'%20lacks. 
 
https://dronebotworkshop.com/tb6612fng-h-bridge/ 
 
https://forum.makeblock.com/t/the-review-of-dc-motor-drivers-l298n-tb6612fng-and-lv8406t/372 
 
 
| Spec | L289N | TB6612FNG | Comments |
|------|-------|-----------|----------|
| Input Voltage | 3.2 - 40 Volts | 15 V MAX | Doesn’t rly matter since we’re using 12V motors anyways. |
| Output Current | 2A continuous current | 1.2A continuous current<br>3.2A peak current | Need to see the current ratings for whatever motor we end up choosing – one of the biggest differences here is the 1.2A vs 2A continuous current rating – for our old motors, this might not matter as much, not sure what this looks like for new motors. |
| Logic Supply Voltage | 4.5 - 7 Volts | 2.7 - 5.5 Volts | Wouldn’t rly matter – we have 12V->5V then 5V->3.3V so either would work. If we use TB6612FNG we could prob just use one 12V->3.3V buck converter. |
| Efficiency | 40-70% | 90-95% | TB6612FNG would prob be wayyy nicer on our battery power supply. Higher efficiency rating + no need for heat sink. |