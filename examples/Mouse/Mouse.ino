// test of USB Host Mouse/Keyboard and send data via I2c port

#include "USBHost_t36.h"
#include <i2c_t3.h>

IntervalTimer myTimer;
int Mspeed = 0;
unsigned int Distance = 32767;
int Distance1 = 0;
uint32_t Distance2 = 0;

//initialize usb host
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
USBHub hub3(myusb);
KeyboardController keyboard1(myusb);
KeyboardController keyboard2(myusb);
USBHIDParser hid1(myusb);
USBHIDParser hid2(myusb);
USBHIDParser hid3(myusb);
USBHIDParser hid4(myusb);
USBHIDParser hid5(myusb);
MouseController mouse1(myusb);
JoystickController joystick1(myusb);

// Function prototypes
void receiveEvent(size_t count);
void requestEvent(void);

// Memory
#define MEM_LEN 256
uint8_t databuf[MEM_LEN];
volatile uint8_t received;
uint8_t TXData[4];
uint8_t RXData[4];

void setup()
{
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("USB Host and send data Testing:");
  
  myusb.begin();
  //myTimer.begin(getDistance, 100000);
 
  pinMode(LED_BUILTIN,OUTPUT); // LED

  // Setup for Slave mode, address 0x19, pins 18/19, external pullups, 400kHz
  Wire.begin(I2C_SLAVE, 0x19, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);

  // Data init
  received = 0;
  memset(databuf, 0, sizeof(databuf));

  // register events
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void getDistance(){
  Distance1 = Distance;
}


void loop()
{
  myusb.Task();
  Distance += mouse1.getMouseX();

  if(mouse1.available()) {
      // print received data - this is done in main loop to keep time spent in I2C ISR to minimum
      inttolitend(Distance, TXData);
      Distance2 = litendtoint(RXData);
      if(received)
      {
        digitalWrite(LED_BUILTIN,HIGH);
        Serial.printf("Slave received:  %d \n", (Distance2 - 32767));
        received = 0;
        digitalWrite(LED_BUILTIN,LOW);
      }

    mouse1.mouseDataClear();
  }

}

//
// handle Rx Event (incoming I2C data)
//
void receiveEvent(size_t count)
{
    Wire.read(RXData, count);  // copy Rx data to databuf
    received = count;           // set received flag to count, this triggers print in main loop
}

//
// handle Tx Event (outgoing I2C data)
//
void requestEvent(void)
{
    Wire.write(TXData, 4); // fill Tx buffer (send full mem)
}

void inttolitend(uint32_t x, uint8_t *lit_int) {
    lit_int[0] = (uint8_t)(x >>  0);
    lit_int[1] = (uint8_t)(x >>  8);
    lit_int[2] = (uint8_t)(x >> 16);
    lit_int[3] = (uint8_t)(x >> 24);
}

uint32_t litendtoint(uint8_t *lit_int) {
    return (uint32_t)lit_int[0] <<  0
         | (uint32_t)lit_int[1] <<  8
         | (uint32_t)lit_int[2] << 16
         | (uint32_t)lit_int[3] << 24;
}
