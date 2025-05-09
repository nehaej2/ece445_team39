# Ramya Work

**3/6/2025**

## Power Subsystem Overview and Calculations:

### Description and Purpose:

Our design has three main power systems, each corresponding to its respective subsystem. The motor subsystem will be powered by a 12 V rechargeable battery. The wagon subsystem will be powered by four 1.5 V AA batteries connected in series, outputting 6 V in total. The user subsystem will also be powered by four 1.5 V AA batteries. We chose to have three separate power systems because each subsystem has its own PCB.

The motor power subsystem is responsible for supplying voltage to one ESP32, one UWB transceiver (DWM1000), the motor driver, two motors, and the USB connection module. To supply the correct voltage to each of these components, we have two systems for stepping down the 12 V source. We first have a buck converter, that steps down 12 V to 5 V. This will supply power to the L298N motor driver chip. We then have a voltage regulator, that steps down the 5 V power supply from the buck converter to 3.3 V. This will power the UWB transceiver, the ESP32, and the CP210N USB to UART master bridge chip.

The user and wagon power subsystems are identical. They are each responsible for supplying voltage to one ESP32 and one UWB transceiver. It will step down the 6V power source to 3.3V utilizing an AP2112K-3.3 fixed voltage converter.

### Parts used:

- 12 V 5200mAh rechargeable battery  
- 8x AA batteries  
- 2x Quad AA battery holder  
- LM2596-5.0 Buck Converter  
- 3x AP2112K-3.3 voltage regulator  

---

### Justifications for each component:

**12 V 5200mAh Battery:**  
For the motors in the motor subsystem, we see a minimum start voltage of 1 V, and an ideal running voltage of 12 V. Due to this operational range, we have decided to use a 12 V battery. We have specifically chosen the KBT 12 V 5200mAh battery (1C), as it is able to handle the max current draw of our components. It also has a max continuous discharge rate of 15.6 A (3C), which means it can handle the worst case stall current of 14A total for both motors. These calculations are detailed below.

As determined in our tolerance analysis, we have calculated a max continuous current draw of 2A per motor. Based on documentation we have also found a max current draw of 40mA from the motor driver module, 352mA for the ESP32, and 160mA for the UWB transceiver. This results in a total current draw of 4.552 A.

```
I_max = 2A + 2A + 0.16A + 0.04A + 0.352A = 4.552A
```

Since the motors have a max stall current of 7A per motor, this results in a total stall current of 14A. Since we are not straining the motors with a hefty load, the motors may only experience a surge in current requirement upon start up. Assuming the motors experience a surge in current usage for a maximum time of 1 minute, we have calculated the max current draw upon start up to be:

```
Time = 1 min / 60 min = 0.0167 hours  
I_drawn = 14A * 0.0167 hours = 0.2333Ah  
5.2Ah - 0.2333Ah = 4.96667Ah remaining battery life
```

Based on this information and the max current draw calculated previously, we will have a battery runtime of:

```
Runtime = Battery Capacity (Ah) / Current Draw (A)  
Runtime = 4.96667Ah / 4.552A ≈ 1.091 hours ≈ 65.46 minutes
```

Since this wagon is a prototype model, a runtime between 45-60 minutes should suffice, and this battery would provide the system with enough power to fulfill that requirement.

**4x AA Batteries:**  
According to the Duracell datasheet, their Optimum AA batteries have a voltage of 1.5 V and a total service hours time of approximately 3 hours when current is being drawn at a constant rate of 500 mA. This can be seen from the image taken from the data sheet below. Since both the Wagon and User systems require approximately 3.3 volts and 0.515 A of current, the AA batteries will provide more than sufficient power for our goal runtime between 45-60 minutes.

![Voltage vs. Service Hours for Duracell Optimum AA Battery Given Constant Current](images/Duracell%20Optimum%20Hours%20vs.%20Voltage.png)

**LM2596-5.0 Buck Converter:**  
Initially, when designing the motor power system, we debated utilizing a standard 5 V voltage regulator capable of handling a 12 V input. After further research, we determined that the heat dissipation from a 7 V voltage drop utilizing a standard voltage regulator would be too much for our system to handle, especially given that we are using sensitive components such as the DWM1000. To properly handle this high voltage drop, we decided to utilize a step-down buck converter, which generates minimal heat. This specific part we have chosen allows for an output current of 1.4A, which will be enough to support the motor driver logic, max current draw from the ESP32, and the max current draw from the UWB transceiver. The schematic for the buck converter implementation from Texas Instruments documentation can be seen below.

![Typical Application Circuit for LM2596-5.0](images/LM2596%20Buck%20Converter%20Schematic.png)

**AP2112K-3.3 Voltage Regulator:**  
This voltage regulator is used in all three power systems in our design. On the motor subsystem, it will convert the 5 V output from the buck converter to 3.3 V. On the user and wagon subsystems, it will convert the 6 V power source to 3.3 V. We decided to utilize a voltage regulator in both cases since the voltage drops are only 1.7 V and 2.7 V, and will not result in high levels of heat dissipation. This chosen voltage regulator is able to provide an output current of 600 mA, which will be sufficient for the max current draw of both the ESP32 and the DWM1000 modules. The schematic for the voltage regulator implementation from Diodes Incorporated can be seen below.

![Typical Application Circuit for AP2112K-3.3](Images/AP2112%20Schematic.png)

---

### High level requirements table/verifications:

| Requirements | Verification |
|--------------|-------------|
| Provide 14 A continuous current for 1 minute. | ● Turn off the motors completely, then power them up using the battery pack. <br> ● Ensure that the battery pack can handle any surge in current draw upon start up. <br> ● Ensure voltage provided by battery stays within 10.5-12 V range after current draw. |
| Provide a stable voltage source of 5 V to the motor driver module. | ● Measure output voltage from 12 V to 5 V buck converter using an oscilloscope to ensure it is providing a steady voltage of 5V ± 0.25 V. |
| Provide a stable voltage of 3.3 V to the ESP32 and DWM1000. | ● Measure output voltage from 5 V to 3.3 V voltage regulator using an oscilloscope to ensure it is providing a steady voltage of 3.3V ± 0.165 V. |
| Ensure the system can handle the current and voltage requirements of all components for between 45-60 minutes. | ● Using an alternate power supply (non-battery), we will measure the current draw using a multimeter for each large component. <br> ● We will then ensure that the total current draw from the system fits within 5.2A, to verify the validity of the chosen 12 V 5200mAh battery supply. |

---

## References

[1] STMicroelectronics, “DUAL FULL-BRIDGE DRIVER,” L298N Data Sheet, [Online]. Available: https://www.st.com/resource/en/datasheet/l298.pdf [Accessed: Feb. 18, 2025].  
[2] Handson Technology, “L298N Dual H-Bridge Motor Driver,” User Guide, [Online]. Available: https://components101.com/sites/default/files/component_datasheet/L298N-Motor-Driver-Datasheet.pdf [Accessed: Feb. 18, 2025]  
[3] DFRobot. (n.d.). Metal DC geared motor w/encoder - 12V 251rpm 18kg.CM Wiki. DFRobot. Available: https://wiki.dfrobot.com/12V_DC_Motor_251rpm_w_Encoder__SKU__FIT0186_ [Accessed: Feb. 12, 2025]  
[4] Texas Instruments, “LM2596 SIMPLE SWITCHER® Power Converter 150-kHz 3-A Step-Down Voltage Regulator,” Data Sheet, [Online]. Available: https://www.ti.com/lit/ds/symlink/lm2596.pdf [Accessed: Mar. 6, 2025]  
[5] Diodes Incorporated, “600mA CMOS LDO REGULATOR WITH ENABLE,” AP2112 Data Sheet, [Online]. Available: https://www.mouser.com/datasheet/2/115/AP2112-271550.pdf [Accessed: Feb. 27, 2025]  
[6] Duracell, “MN1500 Size: AA Alkaline-Manganese Dioxide Battery,” Data Sheet, [Online]. Available: https://www.duracell.com/wp-content/uploads/2016/03/MN15USCT0122-A.pdf [Accessed: Mar. 6, 2025]  
[7] Qorvo, “DW1000 Datasheet,” Data Sheet, [Online]. Available: https://www.qorvo.com/products/d/da007946 [Accessed: Jan. 28, 2025]  
[8] Qorvo, “DW1000 User Manual,” User Manual, [Online]. Available: https://www.qorvo.com/products/d/da007967 [Accessed: Jan. 30, 2025]
