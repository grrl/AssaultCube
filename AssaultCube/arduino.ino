#include <hidboot.h>

// USB device mouse library included with Arduino IDE 1.8.5
#include <Mouse.h>

const byte numChars = 9;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC[numChars] = {0};
int integerFromPC = 0;
int floatFromPC = 0;

boolean newData = false;


// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

int Loop = 1;

class MouseRptParser : public MouseReportParser
{
protected:
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

void MouseRptParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
{
#if 0
  // Demo -- Swap left and right buttons on 4 button Kensington trackball
  if (len > 0) {
    uint8_t button1 = buf[0] & 0x01;
    uint8_t button2 = buf[0] & 0x02;
    uint8_t button3 = buf[0] & 0x04;
    uint8_t button4 = buf[0] & 0x08;
    buf[0] = (buf[0] & 0xF0) | (button1 << 1) | (button2 >> 1) |
                               (button3 << 1) | (button4 >> 1);
  }
#endif
  // Run parent class method.
  MouseReportParser::Parse(hid, is_rpt_id, len, buf);

  Serial.print("MouseRptParser::Parse");
  // Show USB HID mouse report
  for (uint8_t i = 0; i < len ; i++) {
    Serial.print(' ');
    Serial.print(buf[i], HEX);
  }
  Serial.println("END OF THE WALKWAY");

  //Serial.println(" Data Loop = " + String(Loop));
  //Loop++;

  if (len > 2) {
    uint8_t mouseRpt[4];
    mouseRpt[0] = buf[0];
    mouseRpt[1] = buf[1];
    mouseRpt[2] = buf[2];
    // If the mouse/trackball has a scroll wheel, send the value
    if (len > 3) {
      mouseRpt[3] = buf[3];
    }
    else {
      mouseRpt[3] = 0;
    }
    HID().SendReport(1,mouseRpt,sizeof(mouseRpt));
  }
}

USB     Usb;
HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);

MouseRptParser Prs;

void setup()
{
  Serial.begin( 115200 );
  uint8_t attempts = 30;
  while (!Serial && attempts--) {
    delay(100); // Wait for serial port to connect for up to 3 seconds
  }
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
    recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        showParsedData();
        newData = false;
    }
  Usb.Task();
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    //strtokIndx = strtok(tempChars,",");      // get the first part - the string
    //strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC
 
    strtokIndx = strtok(tempChars, ","); // this continues where the previous call left off
    integerFromPC = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    floatFromPC = atoi(strtokIndx);     // convert this part to a float

}

//============

void showParsedData() {
    //Serial.print("Message ");
    //Serial.println(messageFromPC);
    Serial.print("Integer ");
    Serial.print(integerFromPC);
    Serial.print(", ");
    Serial.println(floatFromPC);

	if (integerFromPC == xx && floatFromPC == xx){
	}
	else if (integerFromPC == xx && floatFromPC == xx){
	}
	else{
	
		Mouse.move(integerFromPC, floatFromPC, 0);
	}
    //Serial.print("Integer2 ");
    //Serial.println(floatFromPC);
}