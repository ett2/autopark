/*
--------------------------------------------------------------------------
RF Transceiver (0)
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

/************ Radio Setup ***************/

#define RF69_FREQ 433.0

#define RFM69_CS      4
#define RFM69_INT     3
#define RFM69_RST     6
#define LED           5  

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission

/************ Button Setup **************/
const int b1 = A5;  // mode 1
const int b2 = A4;  // mode 2
const int b3 = A3;  // mode idle
const int b4 = A2;  // mode stop
const int buttonpress = 2; // /button interrupt 

/************ LED Setup *****************/

const int LED1 = 7;
const int LED2 = 8;
const int LED3 = 9;
const int LED4 = 10;

/************ State Setup ***************/

volatile int b1state = LOW;
volatile int b2state = LOW;
volatile int b3state = LOW;
volatile int b4state = LOW;

volatile byte state = 0;

volatile char message = '0';

// ------------------------------------------------------------------------
// Setup
// ------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);

  // Radio setup
  pinMode(LED, OUTPUT);     
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // manual reset
  radio_reset();
  
  
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  // Fob setup
  pinMode(buttonpress, OUTPUT);
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  Serial.println("Fob setup complete!");
  // initialize interrupt
  attachInterrupt(digitalPinToInterrupt(buttonpress), statechange, RISING);
}

// ------------------------------------------------------------------------
// Main Loop
// ------------------------------------------------------------------------

void loop() {
  b1state = digitalRead(b1);
  b2state = digitalRead(b2);
  b3state = digitalRead(b3);
  b4state = digitalRead(b4);
  //delay(1);

  // detects button press and sends message
  if (b1state == HIGH || b2state == HIGH || b3state == HIGH || b4state == HIGH) {
    digitalWrite(buttonpress, HIGH);
    send_message();
    Blink(LED, 40, 3);
    // receive_message();
  }
  else { 
    digitalWrite(buttonpress, LOW);
  }

  delay(1);
  //Serial.println(state);
}

// ------------------------------------------------------------------------
// State Change
// ------------------------------------------------------------------------

void statechange() {
  // calls message function
  if (b1state == HIGH) {
    state = 1;
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
  } else if (b2state == HIGH) {
    state = 2;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
 } else if (b3state == HIGH) {
    state = 3;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, LOW);
  } else if (b4state == HIGH) {
    state = 4;
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, HIGH);
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    }
  digitalWrite(buttonpress, LOW);

}