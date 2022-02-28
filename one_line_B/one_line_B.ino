#include "common.h"
//#define DEBUG
#ifdef DEBUG
  char dbgstr[100];
#endif
// Constants and variables used in Receiver State Machine (RSM)
#define STATE_IDLE      1
#define STATE_START     2
#define STATE_RECEIVE   3
#define STATE_END       4

#define SUCCESS         1
#define ERR_NOT_READY   -1

int RSMpin;
byte RSMbuffer;
byte RSMreceived;
bool RSMisAvailable;
int RSMstate;
int RSMcountRecv;
unsigned long RSMlastTime;

// Functions for the operations of Receiver State Machine (RSM)
void RSMInit(int tx_pin); // Initialization of RSM
bool RSMAvailable(void);  // Check if there is a incoming byte
int RSMRead(void);        // Read the incoming byte. If there is no incoming byte, return -1.
void RSMLoop();           // RSM main loop (should be periodically called)

// Function: RSMInit()
// Description: Initialization of RSM
void RSMInit(int rx_pin)
{
  RSMpin = rx_pin;
  pinMode(RSMpin, INPUT_PULLUP);
  RSMisAvailable = false;
  RSMstate = STATE_IDLE;
#ifdef DEBUG
        Serial.println("RSMInit() called");
#endif        
}

// Function: RSMAvailable()
// Deacription: Check if there is a incoming byte
bool RSMAvailable()
{
#if 0  
#ifdef DEBUG
        Serial.println("RSMAvailable() called");
#endif
#endif        
  return RSMisAvailable;
}

// Function: RSMRead()
// Description: Read the incoming byte. If there is no incoming byte, return -1.
int RSMRead()
{
#ifdef DEBUG
        Serial.println("RSMRead() called");
#endif        
  if(!RSMisAvailable) return -1;
  RSMisAvailable = false;
#ifdef DEBUG
        Serial.println("RSMRead() returnned");
#endif        
  return RSMreceived;
}

// Function: RSMLoop()
// Description: RSM main loop (should be periodically called)
void RSMLoop()
{
  unsigned long current;
  byte inbit;
#if 0
#ifdef DEBUG
    sprintf(dbgstr, "RSMLoop(): RSMstate = %d", RSMstate);
    Serial.println(dbgstr);
#endif
#endif
  switch (RSMstate) {
    case STATE_IDLE:
      if(digitalRead(RSMpin)==LOW) { 
        RSMstate = STATE_START;
        RSMcountRecv = 0;
        RSMlastTime = millis();
#ifdef DEBUG
        Serial.println("STATE_IDLE: get LOW");
#endif
      }        
    break;
    
    case STATE_START:
      current = millis();
      if(current-RSMlastTime >= BIT_INTERVAL+BIT_INTERVAL/2) {
        RSMlastTime = current;
        RSMbuffer = digitalRead(RSMpin);
        RSMcountRecv = 1;
        RSMstate = STATE_RECEIVE;
#ifdef DEBUG
        Serial.println("in STATE_START");
        sprintf(dbgstr,"STATE_START: get first bit: %d", RSMbuffer);
        Serial.println(dbgstr);
#endif        
      }
      break;
      
    case STATE_RECEIVE:
      current = millis();
      if(current-RSMlastTime >= BIT_INTERVAL) {
        RSMlastTime = current;
        if(RSMcountRecv < 8) {
          inbit = digitalRead(RSMpin);
          RSMbuffer |= (inbit << RSMcountRecv);
          RSMcountRecv++;
#ifdef DEBUG
          Serial.println("in STATE_RECEIVE");
          sprintf(dbgstr,"STATE_RECEIVE: get bit %d: %d, buffer=%02X", RSMbuffer, RSMcountRecv, inbit, RSMbuffer);
          Serial.println(dbgstr);
#endif        
        }
        else {
          RSMreceived = RSMbuffer;
          RSMisAvailable = true;
          RSMstate = STATE_END;
#ifdef DEBUG
          Serial.println("STATE_RECEIVE: change to STATE_END");
#endif        
        }
      }
      break;
      
    case STATE_END:
      current = millis();
      if(current-RSMlastTime >= BIT_INTERVAL/2) {
        RSMstate = STATE_IDLE;
#ifdef DEBUG
        Serial.println("STATE_END: change to STATE_IDLE");
#endif
      }        
      break;

    default: {}
  }
#if 0
#ifdef DEBUG
    sprintf(dbgstr, "RSMLoop() End: RSMstate = %d", RSMstate);
    Serial.println(dbgstr);
#endif
#endif        
}

void setup() {
  Serial.begin(115200);
#ifdef DEBUG
  Serial.println("Program Start....");
#endif        
  RSMInit(PIN_RX);
}

// Global variables used in loop()

void loop() {
  char incomingByte;
  if(RSMAvailable()) {  // A byte incoming
    incomingByte = RSMRead();
    Serial.print(incomingByte);
  }
  RSMLoop();
}
