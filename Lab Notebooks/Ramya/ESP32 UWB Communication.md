# 2/19/25: ESP32 and DWM1000 Interfacing 
https://forum.arduino.cc/t/how-to-use-ultra-wideband-bu01-dw1000-without-breakout-board-on-es
p32-d1-mini/1121816/3 
 
->They are talking about using a ESP32 mini, but still some of the comments were talking 
about decoupling capacitors, etc. 
 
https://electropeak.com/learn/interfacing-dwm1000-positioning-module-with-arduino/ 

->This guy basically gives the wiring layout for using DWM1000 with an Arduino Uno. I think 
this will be quite helpful. 

->In one of the original poster’s comments he says: Yes, absolutely! You can use an ESP 
instead of an Arduino. Additionally, with ESP32, you don’t need a logic converter—you 
can directly connect the ESP32 to the DW1000. 

->->I’m unsure if this is actually the case? 
 
https://wiki.makerfabs.com/ESP32_UWB.html 

->Wiki page for the ESP32 with connected UWB transceiver – need to see if there is proper 
documentation on the wiring. 

https://github.com/Makerfabs/Makerfabs-ESP32-UWB/tree/main/hardware 

->I think this is the link to the schematic, but they only give a .sch file so we can try 
uploading it to KiCad and see what info we get. 
 
https://sites.google.com/site/wayneholder/uwb-ranging-with-the-decawave-dwm1000?authuser=0 

->Project on the Arduino Pro Mini with the DWM1000 
 
https://forum.qorvo.com/t/dwm1000-behaving-weird-on-custom-board/15429 

->Person who tried connecting ESP32 to DWM1000 – successfully did this but had some 
issues w/ connectivity. Could try going through his post history to see if they have any 
additionally info 
 
 
https://www.makerfabs.com/blog/post/esp32-uwb-indoor-positioning-test#:~:text=In%20this%20a
pplication%2C%20we%20used,show%20the%20real%2Dtime%20position.  

->Indoor positioning with ESP32 UWB makerfabs


---
