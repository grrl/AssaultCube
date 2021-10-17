/*
 * USB Mouse pass through. Works at HID report level. HID report is not decoded.
 * Tested on Leonardo with Arduino USB host shield, IDE 1.8.5, and Logitech Marble
 * trackball/mouse. This is a proof of concept so is not guaranteed to work with 
 * all USB mice/trackballs.
 */
// USB host mouse from USB Host Shield Library. Install using Library Manager
#include <hidboot.h>

// USB device mouse library included with Arduino IDE 1.8.5
#include <Mouse.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC[numChars] = {0};
int integerFromPC = 0;
int integerFromPC2 = 0;

boolean newData = false;

class MouseRptParser : public MouseReportParser
{
protected:
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

void MouseRptParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
  Serial.print("MouseRptParser::Parse");
  // Show USB HID mouse report
  for (uint8_t i = 0; i < len ; i++) {
    Serial.print(' '); Serial.print(buf[i], HEX);
  }
  Serial.println();

  //int data = Serial.read();
  //Serial.println(data);
  //if(data == 'A')
  //  Mouse.click();

  // On error - return
  if (buf[2] == 1)
    return;

  if (len > 2) {
    uint8_t mouseRpt[4];
    mouseRpt[0] = buf[0];
    mouseRpt[1] = buf[1];
    mouseRpt[2] = buf[2];
    mouseRpt[3] = 0;
    HID().SendReport(1,mouseRpt,sizeof(mouseRpt));
  }
}

USB     Usb;
HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);

MouseRptParser Prs;

void setup()
{
  Serial.begin( 115200 );
#if !defined(__MIPSEL__)
  while (!Serial) delay(1); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");

  if (Usb.Init() == -1) {
    Serial.println("USB host shield did not start.");
  }
  delay( 200 );

  HidMouse.SetReportParser(0, &Prs);

  Mouse.begin();
}

void loop()
{
  if(Serial.available())
  {
    int data = Serial.read();
    if(data == 'A')
      Mouse.click();
  }
  Usb.Task();
}