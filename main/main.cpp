#include "Arduino.h"
#include <Wire.h>
#include <ScioSense_ENS16x.h>
#include <AHTxx.h>

#define I2C_ADDRESS 0x53

#define USE_INTERRUPT
#define INTN 2

ENS160 ens160;
AHTxx aht21(AHTXX_ADDRESS_X38, AHT2x_SENSOR); //sensor address, sensor type

void setup()
{
    Serial.begin(115200);
    //ens160.enableDebugging(Serial);
    Wire.setPins(27,26); // SDA, SCL
    Wire.begin();
    ens160.begin(&Wire, I2C_ADDRESS);
    delay(AHT2X_POWER_ON_DELAY);
    aht21.softReset();
    delay(1000);

    Serial.println("begin..");
    while (ens160.init() != true)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

#ifdef USE_INTERRUPT
    ens160.setInterruptPin(INTN);
    ens160.writeConfiguration
    (
          ENS16X_CONFIGURATION_INTERRUPT_ENABLE
        | ENS16X_CONFIGURATION_NEW_GENERAL_PURPOSE_DATA
        | ENS16X_CONFIGURATION_NEW_DATA
    );
#endif

    ens160.startStandardMeasure();
}

void loop()
{

    // Read data from AHTxx
    float humidity = aht21.readHumidity();
    float temperature = aht21.readTemperature();
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
    ens160.writeCompensation(temperature, humidity);
    delay(1000);
    // Read data from ENS160

     ens160.wait();

     // Enable Tools->Serial Plotter to see the sensor output as a graph

     if (ens160.update() == RESULT_OK)
     {
        if (ens160.hasNewData())
        {
            Serial.print("AQI UBA:"); Serial.print((uint8_t)ens160.getAirQualityIndex_UBA());

            Serial.print("\tTVOC:"); Serial.print(ens160.getTvoc());
            Serial.print("\tECO2:"); Serial.println(ens160.getEco2());
        }

        if (ens160.hasNewGeneralPurposeData())
        {
            Serial.print("RS0:"); Serial.print(ens160.getRs0());
            Serial.print("\tRS1:"); Serial.print(ens160.getRs1());
            Serial.print("\tRS2:"); Serial.print(ens160.getRs2());
            Serial.print("\tRS3:"); Serial.println(ens160.getRs3());
        }
     }
}