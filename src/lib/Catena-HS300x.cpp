/*

Module: Catena-HS300x.cpp

Function:
        Code for Catena HS300x library

Copyright and License:
        See accompanying LICENSE file.

Author:
        Terry Moore, MCCI Corporation   June 2019

*/

#include <Catena-HS300x.h>

using namespace McciCatenaHs300x;

bool cHS300x::begin(void)
    {
    std::uint8_t buf[2];

    this->m_wire->begin();

    /* do a data fetch to see whether we can see the device */
    return this->readResponse(buf, sizeof(buf));
    }

void cHS300x::end(void)
    {
    }

bool cHS300x::getTemperatureHumidity(float &t, float &rh) const
    {
    uint16_t tfrac, rhfrac;
    bool fResult;

    fResult = this->getTemperatureHumidityRaw(tfrac, rhfrac);
    if (fResult)
        {
        t = this->rawTtoCelsius(tfrac);
        rh = this->rawRHtoPercent(rhfrac);
        }
    else
        {
        t = rh = NAN;
        }

    return fResult;
    }

bool cHS300x::getTemperatureHumidityRaw(std::uint16_t &tfrac, std::uint16_t &rhfrac) const
    {
    uint32_t msDelay;
    bool fResult;

    fResult = true;
    msDelay = this->startMeasurement();
    if (msDelay == 0)
        {
        fResult = false;
        }

    if (fResult)
        {
        delay(msDelay);

        uint32_t tStart = millis();
        
        do  {
            fResult = this->getMeasurementResultsRaw(tfrac, rhfrac);
            } while (! fResult && (millis() - tStart) < this->kGetTemperatureTimeoutMs);
        }

    if (! fResult)
        {
        tfrac = rhfrac = 0;
        }
    
    return fResult;
    }

std::uint32_t cHS300x::startMeasurement(void) const
    {
    std::uint8_t error;

    this->m_wire->beginTransmission(this->getAddress());
    error = this->m_wire->endTransmission();

    if (error != 0)
        {
        if (this->isDebug())
            {
            Serial.print("startMeasurement: can't select device: error ");
            Serial.println(unsigned(error));
            }
        return 0;
        }
    else
        {
        return this->kMeasurementDelayMs;
        }
    }

bool cHS300x::getMeasurementResults(float &t, float &rh) const
    {
    uint16_t tfrac, rhfrac;
    bool fResult;

    fResult = this->getMeasurementResultsRaw(tfrac, rhfrac);
    if (fResult)
        {
        t = this->rawTtoCelsius(tfrac);
        rh = this->rawRHtoPercent(rhfrac);
        }

    return fResult;
    }

bool cHS300x::getMeasurementResultsRaw(std::uint16_t &tfrac, std::uint16_t &rhfrac) const
    {
    std::uint8_t buf[4];
    bool fResult;

    fResult = this->readResponse(buf, sizeof(buf));
    if (fResult)
        {
        uint8_t status = buf[0] & 0xC0;
        if (status != 0)
            {
            if (this->isDebug())
                {
                Serial.print("getMeasurementResultsRaw: invalid data status: ");
                Serial.println(unsigned(status >> 6u));
                }
            fResult = false;
            }
        }

    if (fResult)
        {
        tfrac = ((buf[2] << 8) | buf[3]) & 0xFFFCu;
        rhfrac = (buf[0] << 10) | (buf[1] << 2);
        }

    return fResult;
    }

bool cHS300x::readResponse(std::uint8_t *buf, size_t nBuf) const
    {
    bool ok;
    unsigned nResult;
    const std::int8_t addr = this->getAddress();
    uint8_t nReadFrom;

    if (buf == nullptr || nBuf > 32 || addr < 0)
        {
        if (this->isDebug())
            Serial.println("readResponse: invalid parameter");

        return false;
        }

    nReadFrom = this->m_wire->requestFrom(std::uint8_t(addr), /* bytes */ std::uint8_t(nBuf));

    if (nReadFrom != nBuf)
        {
        if (this->isDebug())
            {
            Serial.print("readResponse: nReadFrom(");
            Serial.print(unsigned(nReadFrom));
            Serial.print(") != nBuf(");
            Serial.print(nBuf);
            Serial.println(")");
            }
        }
    nResult = this->m_wire->available();

    for (unsigned i = 0; i < nResult; ++i)
        buf[i] = this->m_wire->read();

    if (nResult != nBuf && this->isDebug())
        {
        Serial.print("readResponse: nResult(");
        Serial.print(nResult);
        Serial.print(") != nBuf(");
        Serial.print(nBuf);
        Serial.println(")");
        }

    return (nResult == nBuf);
    }
