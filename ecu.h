#ifndef ECU_H
#define ECU_H

#include "stopwatch.h"

#include <stdint.h>

#define ECU_PIN        7

struct ECUSample
{
    ECUSample() :
        injectorCentisec(0),
        ignitionAngle(142),
        iac(125),
        rpm(0)
    {
    }

    // Injector pulse width (INJ)
    // X / 10 = mS
    // example: X = 43 = 4.3ms
    uint8_t injectorCentisec;
    // Ignition timing angle (IGN)
    // X - 90 = degrees BTDC
    // example: X = 130 = 40 degrees BTDC
    uint8_t ignitionAngle;
    // Idle air control (IAC)
    // X = step #
    // example: X = 125 =
    uint8_t iac;
    uint8_t rpm;
    uint8_t vaf;
    uint8_t ect;
    uint8_t tps;
    uint8_t speed;

    enum class ECUFlags
    {
        ColdStart = 0x01,
        Warm      = 0x02,
        StartSw   = 0x04,
        IdleSw    = 0x08,
        ACSw      = 0x10,
        NeutralSw = 0x20,
        DiagCond  = 0x40,
    };
    uint8_t flags;

};

// InjectorWidth, Ignition, IAC, Rpm, VAF, ECT, TPS, Speed
class ECU
{
public:
    // Reads the diagnostic port into the sample.
    // Blocks at most ~2.5 seconds (one period to sync with preamble, another to read).
    // Returns true iff sample is read.
    bool readDiagPort(ECUSample& sample);

    static constexpr uint8_t pin = ECU_PIN;

protected:
    // Samples port for 1000usec.
    // Returns true if majority was high.
    bool samplePort(uint16_t* highUsec = nullptr);

    // Preamble of 15 high bits
    bool findPreamble();

    uint16_t readParameter(uint16_t numBits, unsigned long paramTime, bool last);

    Stopwatch diagStopwatch;
    Stopwatch fuelStopwatch;

    inline uint16_t calcSpeed(const ECUSample& sample) const;
    inline uint16_t calcTemp(const ECUSample& sample) const;

    bool m_metric = true;

    static constexpr uint16_t bitUsec = 8192;
    static constexpr uint16_t injFlow = 200;
    static constexpr uint8_t cyl = 6;
    static constexpr uint16_t speedCorr = 1;
};

inline uint16_t ECU::calcSpeed(const ECUSample& sample) const
{
    uint16_t speed = sample.speed;
    if (!m_metric)
    {
        speed *= 0.62;
    }
    speed = speed * speedCorr;
    return speed;
}

uint16_t ECU::calcTemp(const ECUSample& sample) const
{
    const auto V = sample.ect;
    uint16_t temp;

    // 3.4V to 4.3V T = -20 + (4.3-V)*22.22
    // 4.3 == 219.3
    // 3.4 == 173.4
    if (V > 173)
    {
        //temp = -20 + V * ;
    }
    // 2.4V to 3.4V T = 0 + (3.4-V)*20
    // 2.4 = 122.4
    else if (V > 122)
    {
        //temp = 0 + ;
    }
    // 1.5V to 2.4V T = 20 + (2.4-V)*22.22
    // 1.5 = 76.5
    else if (V > 76)
    {
    }
    // 0.9V to 1.5V T = 40 + (1.5-V)*33.33
    // 0.9 = 45.9
    else if (V > 45)
    {
    }
    // 0.5V to 0.9V T = 60 + (0.9-V)*50
    // 0.5 = 25.5
    else if (V > 25)
    {
    }
    // 0.3V to 0.5V T = 80 + (0.5-V)*100
    // 0.3 = 15.3
    else // if (V > 15)
    {
    }
    if (!m_metric)
        temp = (temp * 18 / 10) + 32; // Convert to deg F
    return temp;
}
#endif
