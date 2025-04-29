// currently tag is module #5
// The purpose of this code is to set the tag address and antenna delay to default.
// this tag will be used for calibrating the anchors.

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"
#include "link.h"

//ESP32 pin config
#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4
// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

////ESP32 S3 pin config
//#define SPI_SCK 12
//#define SPI_MISO 13
//#define SPI_MOSI 11
//#define DW_CS 9
//// connection pins
//const uint8_t PIN_RST = 5; // reset pin
//const uint8_t PIN_IRQ = 4; // irq pin
//const uint8_t PIN_SS = 9;   // spi select pin

struct MyLink *anchor_list;
int index_num = 0;
long runtime = 0;
String all_json = "";


// TAG antenna delay defaults to 16384
// leftmost two bytes below will become the "short address"
char tag_addr[] = "7D:00:22:EA:82:60:3B:9C";

void setup()
{
  Serial.begin(115200);

  //init the configuration
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

// start as tag, do not assign random short address

  DW1000Ranging.startAsTag(tag_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
  anchor_list = init_link();
}

void loop()
{
  DW1000Ranging.loop();
  if((millis() - runtime) > 1000){ //checked every 1 second
    make_link_json(anchor_list, &all_json);
    runtime = millis();
  }
}

void newRange()
{
  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  Serial.print(",");
  Serial.println(DW1000Ranging.getDistantDevice()->getRange());

  //Ranging 
  fresh_link(anchor_list, DW1000Ranging.getDistantDevice()->getShortAddress(), DW1000Ranging.getDistantDevice()->getRange(), DW1000Ranging.getDistantDevice()->getRXPower());
}

void newDevice(DW1000Device *device)
{
  Serial.print("Device added: ");
  Serial.println(device->getShortAddress(), HEX);
  //adding the new anchor detected to the link list
  add_link(anchor_list, device->getShortAddress());
}

void inactiveDevice(DW1000Device *device)
{
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
  //delete inactive devices
  delete_link(anchor_list, device->getShortAddress());
}
