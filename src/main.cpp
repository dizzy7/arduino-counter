#include <Arduino.h>
#include <EEPROM.h>
#include <QuadDisplay2.h>
#include <Ultrasonic.h>
#include <EEPROMWearLevel.h>

#define EEPROM_LAYOUT_VERSION 0
#define AMOUNT_OF_INDEXES 1
#define INDEX_RING_BUFFER 0

#define SONIC_TRIGGER_PIN 0
#define SONIC_ECHO_PIN 1
#define DISPLAY_CS_PIN 9

#define DEC_BUTTON_PIN 3
#define INC_BUTTON_PIN 2
#define RESET_BUTTON_PIN 7

// создаём объект класса QuadDisplay и передаём номер пина CS
QuadDisplay qd(DISPLAY_CS_PIN);
Ultrasonic ultrasonic(SONIC_TRIGGER_PIN,SONIC_ECHO_PIN,1000);

volatile unsigned int counter = 0;
bool inRange = false;
int range = 0;

volatile bool decButtonPressed = false;
volatile bool incButtonPressed = false;

void resetCounter() {
    counter = 0;
    EEPROMwl.putToNext(INDEX_RING_BUFFER, 0);
    qd.displayInt(counter);
}

void readCounter() {        
    EEPROMwl.get(INDEX_RING_BUFFER, counter);
    qd.displayInt(counter);    
}

void incCounter() {
    ++counter;
    
    EEPROMwl.putToNext(INDEX_RING_BUFFER, counter);    
    qd.displayInt(counter);
}

void decCounter() {
    if (counter > 0) {
        --counter;
    }    
    
    EEPROMwl.putToNext(INDEX_RING_BUFFER, counter);    
    qd.displayInt(counter);
}

void decInterrupt() 
{    
    decCounter();
    decButtonPressed = true;
}

void incInterrupt() 
{        
    incCounter();
    incButtonPressed = true;
}

void setup()
{   
    qd.begin();

    EEPROMwl.begin(EEPROM_LAYOUT_VERSION, AMOUNT_OF_INDEXES, 1024);    
    readCounter();        
    
    attachInterrupt(digitalPinToInterrupt(DEC_BUTTON_PIN), decInterrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(INC_BUTTON_PIN), incInterrupt, RISING);    
    attachInterrupt(digitalPinToInterrupt(RESET_BUTTON_PIN), resetCounter, RISING);    
}

void loop()
{       
    if (decButtonPressed) {
        delay(1000);
        while (digitalRead(DEC_BUTTON_PIN)) {
            decCounter();
            delay(300);
        }
        decButtonPressed = false;
    }

    if (incButtonPressed) {
        delay(1000);
        while (digitalRead(INC_BUTTON_PIN)) {
            incCounter();
            delay(300);
        }
        incButtonPressed = false;
    }

    range = ultrasonic.Ranging(CM);       

    if (!inRange && range <= 10) {        
        inRange = true;
        incCounter();        
        delay(1000);
    } else if (inRange && range > 10) {        
        inRange = false;
        delay(1000);
    }        

    delay(50);
}
