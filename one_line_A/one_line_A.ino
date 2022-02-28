#include "common.h"
//#define DEBUG
#ifdef DEBUG
char dbgstr[100];
#endif

// Constants and variables used in Sender State Machine (SSM)
#define STATE_IDLE      1
#define STATE_START     2
#define STATE_TRANSMIT  3
#define STATE_END       4

#define SUCCESS         1
#define ERR_NOT_READY   -1

int SSMpin;
byte SSMbuffer;
int SSMstate;
int SSMcountSent;
unsigned long SSMlastTime;

// Functions for the operations of Sender State Machine (SSM)
void SSMInit(int tx_pin); // Initialization of SSM
bool SSMIsReady(void);    // Check if SSM is ready for sending a byte
int SSMSend(byte input);  // Send a byte 
void SSMLoop();           // SSM main loop (should be periodically called)

// Function: SSMInit()
// Description: Initialization of SSM
void SSMInit(int tx_pin)
{
  SSMpin = tx_pin;
  pinMode(SSMpin, OUTPUT);
  digitalWrite(SSMpin, HIGH);
  SSMstate = STATE_IDLE;
}

// Function: SSMIsReady()
// Deacription: Check if SSM is ready for sending a byte
bool SSMIsReady()
{
  if (SSMstate == STATE_IDLE) return true;
  else return false;
}

// Function: SSMSend()
// Description: Send a byte 
int SSMSend(byte input)
{
  SSMbuffer = input;
  SSMcountSent = 0;
  SSMlastTime = millis();
  digitalWrite(SSMpin, LOW);
  SSMstate = STATE_START;
#ifdef DEBUG
  sprintf(dbgstr, "Send byte %02X , change state to STATE_START", SSMbuffer); 
  Serial.println(dbgstr);
#endif
}

// Function: SSMLoop()
// Description: SSM main loop (should be periodically called)
void SSMLoop()
{
  unsigned long current;
  switch (SSMstate) {
    byte outbit;
    case STATE_START:
      current = millis();
      if(current-SSMlastTime >= BIT_INTERVAL) {
        SSMlastTime = current;
        SSMcountSent = 1;
        outbit = SSMbuffer & 0x01;
        if(outbit == 1)
          digitalWrite(SSMpin, HIGH);
        else digitalWrite(SSMpin, LOW);
        SSMbuffer >>= 1;
        SSMstate = STATE_TRANSMIT;
#ifdef DEBUG
        sprintf(dbgstr, "STATE_START: send first bit: %d, change to STATE_TRANSMIT", outbit); 
        Serial.println(dbgstr);
#endif
      }
      break;
      
    case STATE_TRANSMIT:
      current = millis();
      if(current-SSMlastTime >= BIT_INTERVAL) {
        SSMlastTime = current;
        if(SSMcountSent < 8) {
          SSMcountSent++;
          outbit = SSMbuffer & 0x01;
          if( outbit == 1)
            digitalWrite(SSMpin, HIGH);
          else digitalWrite(SSMpin, LOW);
          SSMbuffer >>= 1;
#ifdef DEBUG
          sprintf(dbgstr, "STATE_TRANSMIT: send bit %d: %d", SSMcountSent, outbit); 
          Serial.println(dbgstr);
#endif
        }
        else {
          digitalWrite(SSMpin, HIGH);
          SSMstate = STATE_END;
#ifdef DEBUG
          Serial.println("STATE_TRANSMIT: change to STATE_END");
#endif
        }
      }
      break;
      
    case STATE_END:
      current = millis();
      if(current-SSMlastTime >= BIT_INTERVAL) {
        SSMstate = STATE_IDLE;
#ifdef DEBUG
        Serial.println("STATE_END: change to STATE_IDLE");
#endif
      }
      break;

    default: {}
  }
}

void setup() {
  Serial.begin(115200);
  SSMInit(PIN_TX);
#ifdef DEBUG
  Serial.println("Program Start....");
#endif
}

// Global variables used in loop()
byte buffer[100];
int length = 0;
int current_sent = 0;
bool sending = false;

void loop() {
  byte incomingByte;
  if(!sending) {
    if(Serial.available() > 0) {
      incomingByte = Serial.read();
      buffer[length++] = incomingByte;
      if(incomingByte == '\n') sending = true;
    }
  }
  else {
    if(SSMIsReady()) {
      SSMSend(buffer[current_sent++]);
      if(current_sent==length) {
        length = 0;
        current_sent = 0;
        sending = false;
      }
    }
  }
  SSMLoop();
}
