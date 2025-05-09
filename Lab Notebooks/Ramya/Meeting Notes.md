2/16 Meeting Notes 
 
Assigned Tasks: 
 
Fill out ‘Schedule’ page on 445 website (to schedule TA meetings) 
- 
Finalize parts to order – keep track of this on the Expense spreadsheet. Note down what 
needs to be ordered externally vs through the ECEB shops 
- 
Look into proper documentation for the components in each of our subsystems. Keep 
track of these resources in the shared folder.  
- 
->Component schematics/pin assignments 
- 
->Voltage requirements 
- 
->Required communication protocols 

Check if KiCad has components – if not look into importing schematics/creating custom 
ones. 

Look into how to attach components to the PCB from a distance – motor system, UWB 
transceivers.

---

3/24 Meeting Notes: 
 
Finish Motor Subsystem PCB: 
- 
Look into difference between TB6612FNG motor driver and L298N motor driver 
(EVERYONE – meeting 3/25) 
- 
We used the TB6612FNG motor driver module (breakout board) for our 
breadboard demo – has a simpler design. 

Understand why the current motor driver module is drawing very low levels of current – 
slow motor speed (EVERYONE – meeting 3/25) 
 
Change the 5 V voltage regulator to diff model (RAMYA) 

Copy the USB to UART schematic from the motor subsystem (NEHA) 
 
Requirements: 
 
NEHA: 
- 
100mm x 100mm is MAX SIZE FOR PCB – design should fall within these constraints 
- 
Ensure that the antennas for both ESP32 and DWM1000 hang OFF the board. Refer to 
the user system design. 
- 
Follow ESP32 design doc guidelines

---
