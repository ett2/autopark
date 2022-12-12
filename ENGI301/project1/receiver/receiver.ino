/*
--------------------------------------------------------------------------
RF Receiver
--------------------------------------------------------------------------
License:   
Copyright 2022 Eunice Tan
Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors 
may be used to endorse or promote products derived from this software without 
specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------
This does a few things:
- Gets state information from transceiver
- Outputs this information as an encoder
--------------------------------------------------------------------------
Adapted from:
https://learn.adafruit.com/adafruit-feather-32u4-radio-with-rfm69hcw-module/using-the-rfm69-radio
https://github.com/sparkfun/SparkFun_RadioHead_Arduino_Library
*/


#include <SPI.h>
#include <RH_RF69.h>


// ------------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------------

// Declare frequency; should match that of transceiver
#define RF69_FREQ 433.0

#define RFM69_CS      4
#define RFM69_INT     3
#define RFM69_RST     6
#define LED           7 // need to work on LED when I have the time
#define OUT0          8 // output to BeagleBone
#define OUT1          9 // output to BeagleBone 

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter
int state = 0;
int output = 0;



// ------------------------------------------------------------------------
// Setup
// ------------------------------------------------------------------------

void setup() 
{
  Serial.begin(9600);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  // Setting up I/O
  pinMode(LED, OUTPUT);
  pinMode(OUT0, OUTPUT);  
  pinMode(OUT1, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  digitalWrite(OUT0, LOW);
  digitalWrite(OUT1, LOW);
  digitalWrite(LED, LOW);

  // Test
  Serial.println("Autopark Receiver Test");
  Serial.println();

  // radio reset
  radio_reset();
  
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}


// ------------------------------------------------------------------------
// Main Loop
// ------------------------------------------------------------------------

void loop() {
 if (rf69.available()) {
    // Receiving message 
    uint8_t buf[1];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;

      // Printing message
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      state = (int) buf[0];
      Serial.println(state);
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);
      Blink(LED, 40, 3);

      // State Change
      state_change(state);

      // Sending Reply (can change later)
      if (strstr((char *)buf, "Hello World")) {
        // Send a reply!
        uint8_t data[] = "And hello back to you";
        rf69.send(data, sizeof(data));
        rf69.waitPacketSent();
        Serial.println("Sent a reply");
        Blink(LED, 40, 3); //blink LED 3 times, 40ms between blinks
      }
    } else {
      Serial.println("Receive failed");
    }
  }
}


// ------------------------------------------------------------------------
// Manual Reset
// ------------------------------------------------------------------------

void radio_reset() {
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
}


// ------------------------------------------------------------------------
// State Change
// ------------------------------------------------------------------------

void state_change(int s) { // state
  switch (s) {
    // IDLE STATE
    case 0:
      output = 0;
      // 00
      digitalWrite(OUT1, LOW);
      digitalWrite(OUT0, LOW);
      break;
    // PARKING STATE
    case 205:
      output = 1;
      // 01
      digitalWrite(OUT1, LOW);
      digitalWrite(OUT0, HIGH);
      break;
    // PICKUP STATE
    case 247:
      output = 2;
      // 10
      digitalWrite(OUT1, HIGH);
      digitalWrite(OUT0, LOW);
      break;
    // EMERGENCY STOP STATE
    case 252:
      output = 3;
      // 11
      digitalWrite(OUT1, HIGH);
      digitalWrite(OUT0, HIGH);
      break;
  }

}


// ------------------------------------------------------------------------
// Indicatior LED
// ------------------------------------------------------------------------

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i=0; i<loops; i++)  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}
