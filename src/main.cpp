#include <Arduino.h>
#include <EEPROM.h>
#include <QuadDisplay2.h>
#include <Ultrasonic.h>
#include <EEPROMWearLevel.h>

#define EEPROM_LAYOUT_VERSION 0
#define AMOUNT_OF_INDEXES 1
#define INDEX_RING_BUFFER 0

// создаём объект класса QuadDisplay и передаём номер пина CS
QuadDisplay qd(9);
Ultrasonic ultrasonic(0,1,1000);

volatile int counter = 0;
bool inRange = false;
int range = 0;
int eepNextAddress = 0;
int eepCurrentValue = 0;

void clearEep() {
    for (int i = 0; i<= 1023; ++i) {
        EEPROM[i] = 0;
    }
}

void resetCounter() {
    ++eepCurrentValue;
    if (eepCurrentValue > 255) {
        eepCurrentValue = 0;
    }
    EEPROM[0] = ++eepCurrentValue;
    eepNextAddress = 1;
}

void readCounter() {
    eepCurrentValue = EEPROM[0];
    for (int i = 1; i <=1023; ++i) {
        if (EEPROM[i] == eepCurrentValue) {
            ++counter;
        } else {
            eepNextAddress = i;

            return;
        }
    }
    
    resetCounter();
}

void incCounter() {
    ++counter;
    EEPROM[eepNextAddress] = eepCurrentValue;
    ++eepNextAddress;
}

void setup()
{
    qd.begin();
    
    readCounter();

    qd.displayInt(counter);      
}

void loop()
{ 
    range = ultrasonic.Ranging(CM);    

    if (!inRange && range <= 10) {
        Serial.println(range);
        inRange = true;
        incCounter();
        qd.displayInt(counter);
        delay(300);
    } else if (inRange && range > 10) {
        Serial.println(range);
        inRange = false;
        delay(300);
    }        

    delay(50);
}

