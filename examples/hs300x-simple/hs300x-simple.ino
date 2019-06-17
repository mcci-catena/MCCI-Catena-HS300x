/*

Module: hs300x-simple.ino

Function:
        Simple example for HS300x sensor.

Copyright and License:
        See accompanying LICENSE file.

Author:
        Terry Moore, MCCI Corporation   June 2019

*/

#include <Catena-HS300x.h>

#include <Arduino.h>
#include <Wire.h>

/****************************************************************************\
|
|   Manifest constants & typedefs.
|
\****************************************************************************/

using namespace McciCatenaHs300x;

/****************************************************************************\
|
|   Read-only data.
|
\****************************************************************************/

/****************************************************************************\
|
|   Variables.
|
\****************************************************************************/

cHS300x gHs300x {Wire};

/****************************************************************************\
|
|   Code.
|
\****************************************************************************/

void setup()
    {
    Serial.begin(115200);

    // wait for USB to be attached.
    while (! Serial)
        yield();

    Serial.println("HS300x Simple Test");
    if (! gHs300x.begin())
        {
        for (;;)
            {
            Serial.println("gHs300x.begin() failed\n");
            delay(2000);
            }
        }
    }

void loop()
    {
    float t, rh;

    if (! gHs300x.getTemperatureHumidity(t, rh))
        {
        Serial.println("can't read T/RH");
        }
    else
        {
        Serial.print("T(F)=");
        Serial.print(t * 1.8 + 32);
        Serial.print("  RH=");
        Serial.print(rh);
        Serial.println("%");
        }

    delay(1000);
    }
