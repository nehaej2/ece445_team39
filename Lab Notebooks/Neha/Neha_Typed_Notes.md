#1/28/25 
1.  Research existing solutions that are similar/the same  
a.  If they exist, how can we create something thats different  
i.  Price/functionality/different sensors  
2.  Research on what components that are needed  
a.  At a high level overview, draw out and list each component  
b.  For each subsystem, list each component  
 
Fire extinguisher w smoke/heat sensors  
1.  There are already exterior sprinkler system activated by heat sensors  
a.  Sprinkler system flaws: water supply dependency, could be unavailable during power 
outages/water shortages… 
 
 
#2/3/25 
BLE tracking:  
-  Utilize BLE beacon and and BLE receivers to approximate distance between beacon/receiver.  
-  BLE uses RSSI for outdoor positioning. The smartphone estimates the distance to the beacon by 
measuring the strength of the RSSI signal from the beacon.  
 
RFID tracking: 
-  User carries an active RFID tag and wagon has active RFID reader, there needs to be an active 
RFID antenna  
-   
-  There’s two types: passive and active 
-  Active is battery powered can track the asset’s location in real time 
-  https://www.link-labs.com/blog/facts-active-rfid-technology-rtls  
 
Ultrasonic tracking:  
-  User carries an ultrasonic emitter that periodically sends sound pulses and wagon carries 
ultrasonic receivers that detect the pulses and calculate the distance using Time of Flight (ToF) 
 
BLE tracking   
Pros  
●  Low power consumption  
●  Cost effective  
●  Simplicity 
Cons
●  Signal interference (BLE signals 
   can be interfered by any object
   that obscures the line of sight)
●  More examples of indoor
   tracking and may not be accurate
   if the setup depends on
environmental factors
●  Signal strength drops quickly for
   the first 1-2 meters away
●  Slow response time. 

Active RFID tracking  
●  Real time tracking  
●  Better accuracy than BLE RSSI alone  
●  Environmental factors can cause 
  interference problems w tags mounted 
  on metal or on containers w liquid. But 
  improvements in tags and antenna 
  design have allowed these problems to 
  be overcome 
●  May need multiple readers for accuracy 
  purposes  
 
Ultrasonic tracking    
Pros  ●  Directional detection    
●  Accurate for shorter distances  
Cons  ●  Limited range <10m   
●  Interference from noise 
●  Delays due to the fact that 
ultrasonic waves travel slower 
than RF signals  

 
 
Response to TA questions: 
 
The GPS module we plan on using has a positional accuracy of 1.8m. However, we could utilize other 
GPS modules that offer accuracy within 1.5m, depending on how this fits into the project budget. This 
positional accuracy should be good enough for long-range outdoor navigation– since we plan on testing in 
an open outdoor space, this should be fine. However, we could utilize an ultrasonic sensor to improve 
short-range tracking accuracy and potential obstacle detection. This ensures greater tracking accuracy 
since the ultrasonic sensor can more accurately determine distance from the user within a shorter range. 
 
For example, let us consider a situation where an obstacle blocks the robot while the human walks away. 
In the time it takes for the robot to detect and move around a potential obstacle, the human might exceed 
the range of the ultrasonic sensor. The GPS module ensures that even if the human exceeds the ultrasonic 
range, the robot can still move toward the user until it is again in range of the ultrasonic sensor.  
 
#2/10/25 - project proposal  
Solution: 
Our proposed solution is an autonomous, self-following wagon designed to carry loads for users in both 
indoor and outdoor environments. The wagon will employ an Ultra-Wideband (UWB) tracking system to 
accurately follow the user without requiring direct manual control. This system will consist of a UWB 
Tag and Anchor setup, where two UWB tags will be mounted on the wagon, and the user will carry a 
remote with an embedded UWB anchor. By utilizing the Two-Way Ranging (TWR) method, the system 
will determine the user's real-time position and distance through Time of Flight (ToF) calculations. 
 
To enhance navigation stability, the wagon will incorporate additional sensors such as IMUs (Inertial 
Measurement Units) for motion estimation and ultrasonic or LiDAR sensors to detect obstacles and 
prevent collisions. A combination of onboard processing and real-time feedback loops will ensure 
responsive and reliable tracking, even in dynamic environments. This solution provides a hands-free 
alternative to traditional rolling carts and backpacks, reducing physical strain while seamlessly adapting 
to urban and campus landscapes.  
 
 
Wagon Control Subsystem Overview: 
 
The Wagon Control Subsystem comprises UWB anchors that communicate with the user's UWB tag and 
an ESP32 microcontroller, which receives distance data from the user's tag and sends navigation 
commands to the motor subsystem. 
 
This subsystem consists of two key components: two DWM1000 UWB transceivers and an ESP32 
module. The DWM1000 transceivers serve as anchors, while the user's DW100 functions as a tag, 
utilizing a two-way ranging method to determine the user's absolute position relative to the wagon. 
 
Once the ESP32 module receives distance data, it calculates the necessary motor movements. Based on 
the computed distance, the system adjusts the voltage supplied to the motor subsystem to regulate speed 
via RPM and wheel torque. Additionally, if a turn or slight course correction is needed, the system 
determines the appropriate motor angle and adjusts the right and left wheel speeds accordingly to ensure 
precise navigation. 
 
 
Wagon Control Subsystem requirements: 
●  2.8 to 3.6V 
 
#2/20/25 
Questions to Answer: 
 
1.  How do we configure the anchors to communicate with a specific tag? How are the anchors 
programmed?  
 
The actual programming of the Two way ranging algorithm calculation is done by an external 
program(we would have to program the chip using arduino or something), but to configure the UWB 
transceivers to be tags/anchors we would have to put it in either the Tx or the Rx state. The Tx puts the 
transceiver into transmitting mode and it will automatically transmit the data into the RF channel (we 
need to set the two transceivers to be on the same channel). The Rx state is when the receiver is active, the 
RF synthesizer is active.  
 
After these two states we set certain bits and make it so that it loops and does this again.  
https://www.sunnywale.com/uploadfile/2021/1230/DW1000%20User%20Manual_Awin.pdf  
(Page 10) 
 
2.  Does every UWB transceiver need a microcontroller? 
 
https://forum.qorvo.com/t/do-i-need-a-separate-microcontroller-for-each-dwm3000-module/16650 Yeah 
they do.  
  
3.  Current main question: How do we do breadboard demo with Dw1000 and ESP32?  
4.  How do we make the tag communicate with two anchors (switching between two anchors)?  
 
5.  Assuming the DWM1000 needs to be attached to an ESP32, how do we make this connection? 
Can we wire directly? Can we connect two DW1000 to 1 ESP32? 
a.  Do we have enough SPI pins on the ESP32?  
 
Notes: 
-  Page 208 of the user manual talks about antenna calibration – we need to do this to get the t_delay 
value required for TOF calculation 
 
#2/25/25  
MOTORS:  GB37Y3530-131EN  DC12V 83 RPM  
Motor Datasheet: https://wiki.dfrobot.com/12V_DC_Motor_251rpm_w_Encoder__SKU__FIT0186_  
Stall Current: 7A 
RPM: 83 
Gear Ratio: 131:1 
Stall Torque: 45 kg-cm  
12 V DC 
 
Individual component: (Breadboard version?) 
https://www.sparkfun.com/full-bridge-motor-driver-dual-l298n.html 
 
Motor Driver Module pin out: 
https://components101.com/modules/l293n-motor-driver-module 
 
User Guide: (schematic on page 2)  
https://components101.com/sites/default/files/component_datasheet/L298N-Motor-Driver-Datasheet.pdf 
 
Wiki page on the Module: 
http://wiki.sunfounder.cc/index.php?title=Motor_Driver_Module-L298N 
 
Interfacing with the Module (on code): 
https://lastminuteengineers.com/l298n-dc-stepper-driver-arduino-tutorial/ 
 
Full circuit schematic: 
https://newscrewdriver.com/2021/01/28/circuit-schematic-of-generic-l298n-driver-board/ 
 
Motor speed control: 
https://lastminuteengineers.com/l298n-dc-stepper-driver-arduino-tutorial/  
 
 
H-bridge means half bridge.  
-  2 Motor Drive 
-  One high side driver and one low side driver 
Full-bridge  
-  2 low side and 2 high side  
-  Can drive all 4 FETs 
 
 
#2/27/25 
Components needed:  
-  Full Bridge Motor Dual Driver L298N x1  
-  Datasheet: 
https://cdn.sparkfun.com/assets/7/1/d/6/c/Full-Bridge_Motor_Driver_Dual_-_L298N.pdf  
-  Protection Diodes (M7 Diodes) x8 
-  $0.1 
https://www.mouser.com/ProductDetail/Diotec-Semiconductor/M7?qs=OlC7AqGiEDln9i
tgkDD%2F2A%3D%3D&srsltid=AfmBOopetNG_vdVJnx36AFCi_bSYkSmZGrPVXzR
ly02pmi6fpFex714t  
-  Datasheet: https://diotec.com/request/datasheet/m1.pdf  
-  220UF/35V Capacitor x1 
-  5 Volt power system 
 
H-Bridge Motor Driver 
https://www.amazon.com/WWZMDiB-L298N-H-Bridge-Controller-Raspberry/dp/B0CR6BX5QL/ref=sr
_1_6?crid=2SGA46D4C39HG&dib=eyJ2IjoiMSJ9.XVQEXNFwGCIYsosoS8koqtuhs78Tub9IGjNy7JPc
4XEofTJkuXZlQoS3ASJQMOAQbnSSk4y9HoqFG_4cNaK9sCUWxSg0kvk2tp4rBjIIhhQK8XJfAkbA0
y5fgUziK_BxA1Gss5buFs-PFrlbUWUyQBOJRasH2aqI5jPCnkzM1vZYkIVVy8s4d95JtrK4bhzC2LSQV
MfXuPmLj_ceAeCzCnfdbMUQneMre2x56FlRRvQ.3SCE27z7_KFpNONNO2WZzIwL4HxJj1f_Nx7Ut
HXSuok&dib_tag=se&keywords=L298N+Driver&qid=1728253365&sprefix=%2Caps%2C1152&sr=8-6  
 
 
This project used the H-Bridge Motor Driver we wanted to use: 
https://courses.grainger.illinois.edu/ece445/getfile.asp?id=24133  
 
 
Stepper Motor Driver:  
https://www.digikey.com/en/products/detail/adafruit-industries-llc/4489/11594498?utm_adgroup=&utm_s
ource=google&utm_medium=cpc&utm_campaign=PMax%20Shopping_Product_Medium%20ROAS%2
0Categories&utm_term=&utm_content=&utm_id=go_cmp-20223376311_adg-_ad-__dev-c_ext-_prd-115
94498_sig-Cj0KCQjw3vO3BhCqARIsAEWblcAeEV4CUVPWmpnSWrHpN9WDno1HTnAIdZKQamv
xb47h1NqnYRFUwT8aAunoEALw_wcB&gad_source=1&gclid=Cj0KCQjw3vO3BhCqARIsAEWblcAe
EV4CUVPWmpnSWrHpN9WDno1HTnAIdZKQamvxb47h1NqnYRFUwT8aAunoEALw_wcB  
 
Receives and sends PWM signals to motors through GPIO with the microcontroller  
Used in this project:  
Design doc 1: https://courses.grainger.illinois.edu/ece445/getfile.asp?id=24040  
 
Design doc 2: http://courses.grainger.illinois.edu/ece445/getfile.asp?id=24040  
And used with an ESP32 WROOM  
 
 
#3/13/25 
What we need: 
-  DW1000 transceivers x3 
-  https://www.mouser.com/ProductDetail/Qorvo/DWM1000?qs=TiOZkKH1s2R6b5D6df6
3Pg%3D%3D&srsltid=AfmBOorBgtauz42qeSWwc1MVElrz6H12brRAwZ8GtAjdpY5X
UC7XPbOq 
 
-  ESP32 microcontrollers x3 (each DW1000 needs a ESP32 to be the host controlling it, we can 
also look into if two DW1000s can share 1 ESP32) 
 
ESP32 S3 Datasheet: 
Description: This is the datasheet for the ESP module that we are given through ECE Services Shop.  
ESP32 Data Sheet.pdf 
 
ESP32 booting/programming mode info: 
https://docs.espressif.com/projects/esptool/en/latest/esp32s3/advanced-topics/boot-mode-selection.html  
 
ESP32 Development Board datasheet: 
https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/index.html  
 
White paper on DW1000 and info on Two Way Ranging (TWR): 
Description: This gives a high level overview of the different methods of calculating distance via UWB. 
We are going to use the TWR method, so especially look into the explanation for that. 
https://drive.google.com/file/d/1Ppn1IBI56fCGVgKSOGzOwFU6u5n-mIek/view?usp=drive_link 
 
Implementation of Two Way Ranging: 
Description: This essentially goes through all the details of calculating the distance via TWR, error 
calculations, etc. 
https://drive.google.com/file/d/18vUgHqiRZF4ZBTdPLbRQ-HZSJ1Wk53QL/view?usp=drive_link 
 
DWM1000 Datasheet: 
Description: This is the actual UWB module that we would be using – a lot of the detailed 
documentation for this is the same as the DW1000, which is why we’ve linked that below too. You’ll see 
that they mention the DW1000 documentation throughout this datasheet. 
https://drive.google.com/file/d/1FAfgHGcTzXG0a5w5Ea_sFXYIZPdHiaMV/view?usp=drive_link 
 
DW1000 User Manual: 
https://www.sunnywale.com/uploadfile/2021/1230/DW1000%20User%20Manual_Awin.pdf  
 
DW1000 Datasheet: 
https://docs.ai-thinker.com/_media/uwb/docs/dwm1000-datasheet-1.pdf  
 
Makerfabs DW1000 ESP32 in one:  
https://www.makerfabs.com/esp32-uwb-ultra-wideband.html  
 
ESP32 + USB connection Research 
https://www.youtube.com/watch?v=UP0-w7tPxF0  
 
https://mp1993.de/esp-32-programmer-with-cp2102/  
 
CP2102N datasheet around 6 dollars (QFN28) 
https://www.silabs.com/documents/public/data-sheets/cp2102n-datasheet.pdf  
 
#4/21/25 
4/21  
 
 Integrate bluetooth communication from tag to motor subsystem  
 Have transmitted data appear on serial monitor  
 Retest motor subsystem with breadboard demo code (wagon fixed from machine shop)  
 Integrate second anchor  
 Have two distances be received by the motor ESP32  
4/22  
 Integrate bluetooth communication from tag to motor subsystem  
 Have transmitted data appear on serial monitor  
 Normalize UWB distance data  
 
 
__________________________________ 
DETAILED LIST functionality breadboard  
 
UWB: 
 Filter UWB distances (By Sunday 4/27) 
 Test Monday 4/28  
 Integrate BLE and UWB code (By Sunday 4/27) 
 
Motors (fuck you btw): 
Option 1: 
 Go to machine shop ask how to replace motors (By Friday 4/25) 
 Order new motors (By Friday 4/25) 
 Bring the wagon in with new motors (Monday 8 AM!) 
 Test replacement motors of the same type (By Sunday) 
 Get new battery packs & voltage regulator (by Friday) 
  Option 2 (By Friday 4/25 Neha): 
 Write code that compensates for the lag in one wheel and make it go in a straight 
line 
 Turns  
 If doesn’t work, try PID loop/low pass filtering 
 
 Integrate motor code with UWB code (By Monday 4/28) 
 Fix errors, limit to a certain range for turns/following distance  
 
Final step: 
 Move everything onto a smaller breadboard  
 Packaging for the User component TAG from machine shop  
______________________________________ 
Other Work DONT DO UNTIL SATISFIED WITH FUNCTIONALITY : 
 PCB power system testing  
 Programming ESP32 with programmer board 
 
 
