
#define MY_DEBUG
#define MY_RADIO_RFM69
#define MY_IS_RFM69HW
#define MY_NODE_ID 1

#include <SPI.h>
#include <Wire.h>
#include <MySensors.h>
#include <Bounce2.h>
#include "SparkFunTMP102.h"


#define MY_RFM69_FREQUENCY RF69_915MHZ
#define MY_RF69_IRQ_PIN D1
#define MY_RF69_IRQ_NUM MY_RF69_IRQ_PIN
#define MY_RF69_SPI_CS  D8

#define SN "Window Sensor"
#define SV "1.4"


#define REED_ID 3
#define TEMP_ID 4

#define SWITCH_PIN 3
//#define ALERT_PIN A3

Bounce debouncer = Bounce();
int oldValue=-1;

TMP102 temp(0x48); //Initialize TMP102 at address 0x48

MyMessage reedMsg(REED_ID, V_TRIPPED);
MyMessage tempMsg(TEMP_ID, V_TEMP);


void setup()
{
  // Setup gateway communications
  sendSketchInfo(SN, SV);

  // Setup the button
  pinMode( SWITCH_PIN, INPUT );
  // Activate internal pull-up
  digitalWrite( SWITCH_PIN, HIGH );

  // Setup the debouncer
  debouncer.attach( SWITCH_PIN );
  debouncer.interval(5);

  // Setup temp sensor
  temp.setFault(0);          //Trigger alarm immediately
  temp.setAlertPolarity(1);  // Active high
  temp.setAlertMode(0);      //Comparator mode
  temp.setConversionRate(2); //4Hz
  temp.setExtendedMode(0);   // readings from -55C to 128C
  temp.setHighTempF(85.0);   // raise Alert at 85F
  temp.setLowTempF(40.0);    // raise Alert at 32F

  // Present the sensors to our gateway
  present( REED_ID, S_DOOR );
  present( TEMP_ID, S_TEMP );
  send( reedMsg.set(0) );
}


void loop()
{
  //Wake the sensor up to start reading the temp
  temp.wakeup();

  if (debouncer.update())
  {
    int value = debouncer.read();
    send( reedMsg.set(value == LOW ? 1 : 0));
  }

  //Read the temperature
  float temperature = temp.readTempF();

  //Send the temperature to gateway
  send( tempMsg.set(temperature, 1) );

}
