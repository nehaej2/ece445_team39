# Neha - All Typed Notes

## 1/28/25
### 1. Research existing solutions that are similar/the same  
- If they exist, how can we create something that’s different  
  - Price / functionality / different sensors  

### 2. Research on what components are needed  
- High-level overview, draw out and list each component  
- For each subsystem, list each component  

**Example**: Fire extinguisher with smoke/heat sensors  
- Existing: exterior sprinkler system activated by heat sensors  
  - Flaws: water supply dependency, not available during outages/shortages  

---

## 2/3/25
### BLE Tracking
- BLE beacon + BLE receivers to estimate distance via RSSI  
- Pros:
  - Low power
  - Cost effective
  - Simple
- Cons:
  - Signal interference
  - Indoor-focused
  - Short-range accuracy limitations  

### RFID Tracking
- Active RFID tag with active reader  
- Two types: Passive & Active  
- Active tags allow real-time tracking  
- [Link-labs Active RFID Info](https://www.link-labs.com/blog/facts-active-rfid-technology-rtls)  
- Pros:
  - Real-time tracking
  - More accurate than BLE
- Cons:
  - Environmental interference
  - Multiple readers needed  

### Ultrasonic Tracking
- Ultrasonic emitter on user, receivers on wagon  
- Uses ToF (Time of Flight)  
- Pros:
  - Directional, short-range accuracy  
- Cons:
  - Limited range (<10m)
  - Noise interference
  - Slower than RF  

---

## Response to TA Questions

- GPS module accuracy: ~1.8m, possibly upgradeable to 1.5m  
- Use GPS for long-range; ultrasonic for short-range + obstacle avoidance  
- GPS + ultrasonic = hybrid tracking  
- Handles user moving beyond ultrasonic range  

---

## 2/10/25 – Project Proposal

**Solution**:  
Autonomous wagon using UWB tracking  
- UWB setup:
  - 2 UWB tags on wagon
  - 1 anchor carried by user  
- Uses Two-Way Ranging (TWR) via Time of Flight (ToF)  

**Additional Features**:  
- IMUs for motion
- Ultrasonic/LiDAR for collision avoidance
- Responsive tracking + hands-free user interaction  

---

## Wagon Control Subsystem

- **Components**: 2 DWM1000 UWB transceivers + 1 ESP32  
- UWB anchor (Tx) communicates with user tag (Rx)  
- ESP32:
  - Receives distance data
  - Sends voltage commands to motor
  - Adjusts RPM/torque and wheel speeds  

**Requirements**:  
- Voltage: 2.8V to 3.6V  

---

## 2/20/25 – Questions

1. **Configuring anchors & programming**
   - Tx = transmit mode, Rx = receive mode
   - Must be on the same channel  
   - Use loops + bit settings for operation  
   - [DW1000 Manual (Page 10)](https://www.sunnywale.com/uploadfile/2021/1230/DW1000%20User%20Manual_Awin.pdf)

2. **Does each UWB need a microcontroller?**  
   - Yes ([Forum Answer](https://forum.qorvo.com/t/do-i-need-a-separate-microcontroller-for-each-dwm3000-module/16650))

3. **Breadboard demo with DW1000 + ESP32?**  
4. **Multiple anchors communication?**  
5. **Connection + SPI questions**  
   - 2 DW1000s on 1 ESP32?
   - Enough SPI pins?  

**Note**:  
- Antenna calibration required (see user manual pg 208)  

---

## 2/25/25 – Motors

- **Motor**: GB37Y3530-131EN DC12V 83 RPM  
- [Datasheet](https://wiki.dfrobot.com/12V_DC_Motor_251rpm_w_Encoder__SKU__FIT0186_)  
- Specs:
  - Stall Current: 7A
  - Torque: 45 kg-cm
  - Gear Ratio: 131:1

**Motor Driver Module**  
- L298N Full Bridge Motor Driver  
- [Pinout](https://components101.com/modules/l293n-motor-driver-module)  
- [Datasheet](https://components101.com/sites/default/files/component_datasheet/L298N-Motor-Driver-Datasheet.pdf)  
- [Tutorial](https://lastminuteengineers.com/l298n-dc-stepper-driver-arduino-tutorial/)  

---

## 2/27/25 – Components

- **Motor Driver**: L298N  
  - [Datasheet](https://cdn.sparkfun.com/assets/7/1/d/6/c/Full-Bridge_Motor_Driver_Dual_-_L298N.pdf)
- **Protection Diodes**: M7 Diodes x8  
  - [Buy Link](https://www.mouser.com/ProductDetail/Diotec-Semiconductor/M7)  
- **Capacitor**: 220UF/35V x1  
- 5V power system  

**Project Reference**:  
- [ECE Project](https://courses.grainger.illinois.edu/ece445/getfile.asp?id=24133)  

**Stepper Motor Driver**:  
- [Adafruit 4489](https://www.digikey.com/en/products/detail/adafruit-industries-llc/4489/11594498)  
- Used with PWM + GPIO from ESP32  

---

## 3/13/25 – Needs

- DW1000 Transceivers x3  
  - [Buy Link](https://www.mouser.com/ProductDetail/Qorvo/DWM1000)
- ESP32 microcontrollers x3  
- ESP32 S3:
  - [Datasheet](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/index.html)
  - [Boot Mode Info](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/advanced-topics/boot-mode-selection.html)

**Two Way Ranging Info**:
- [TWR Whitepaper](https://drive.google.com/file/d/1Ppn1IBI56fCGVgKSOGzOwFU6u5n-mIek/view)
- [TWR Implementation](https://drive.google.com/file/d/18vUgHqiRZF4ZBTdPLbRQ-HZSJ1Wk53QL/view)

**Datasheets**:  
- [DWM1000 Datasheet](https://drive.google.com/file/d/1FAfgHGcTzXG0a5w5Ea_sFXYIZPdHiaMV/view)
- [DW1000 Manual](https://www.sunnywale.com/uploadfile/2021/1230/DW1000%20User%20Manual_Awin.pdf)

---

## 4/21/25 – Integration Tasks

- Integrate BLE from tag to motor subsystem  
- Confirm data on Serial Monitor  
- Retest motor subsystem  
- Integrate second anchor  

---

## 4/22/25 – Development Plan

- Normalize UWB data  
- Test integrated BLE + UWB  
- Motor Fix Options:
  - Replace motors (machine shop)
  - Write compensation code
  - Use PID / low pass filters  

---

## Final Steps
- Integrate UWB + motor code  
- Fix errors + constrain following range  
- Finalize breadboard  
- Package user tag  

---

## Other Work (Hold until system stable)

- PCB power testing  
- ESP32 programming with CP2102  
- [CP2102 Datasheet](https://www.silabs.com/documents/public/data-sheets/cp2102n-datasheet.pdf)  

---




