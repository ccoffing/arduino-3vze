#include "ecu.h"


bool ECU::samplePort(uint16_t* highUsec)
{
    // Sample the port repeatedly for 1ms to reduce noise.
    uint16_t samples = 0;
    uint16_t high = 0;
    uint32_t elapsed;
    Stopwatch sw;
    do
    {
        samples++;
        if (digitalRead(pin) != LOW)
            high++;
        elapsed = sw.elapsed();
    } while (elapsed < 1000);

    // Avoid overflow (get about 121 samples in 1ms)
    if (highUsec)
        *highUsec = 100 * high / samples * 10;
    return high * 2 >= samples;
}

bool ECU::findPreamble()
{
    Stopwatch timeout;
restart:
    Serial.println(F("looking for preamble"));
    // Wait for high logic level to start
    uint16_t highUsec = 0;
    uint16_t prevHighUsec;
    while (true)
    {
        prevHighUsec = highUsec;
        if (samplePort(&highUsec))
            break;
        // Each cycle is <1.25 seconds.
        if (timeout.elapsed() > 1300000UL)
        {
            Serial.println(F("preamble timeout"));
            return false;
        }
    }
    diagStopwatch.start();
    diagStopwatch.rebase(-prevHighUsec - highUsec);

    Serial.println("found high");
    bool found = false;
    bool high;
    do
    {
        prevHighUsec = highUsec;
        high = samplePort(&highUsec);
        if (diagStopwatch.elapsed() >= 15 * bitUsec && high)
        {
            found = true;
        }
    } while (high);
    if (!found)
        goto restart;
    // Reset timer to reference end of preamble.
    diagStopwatch.start();
    // Discount the time the signal has been low
    diagStopwatch.rebase(- (1000 - prevHighUsec) - (1000 - highUsec));
    Serial.println(F("found preamble"));
    return true;
}

uint16_t ECU::readParameter(uint16_t numBits, uint32_t paramStart, bool resync)
{
    // Wait for the midpoint of each bit to sample.
    uint16_t param = 0;
    uint32_t bitTime = paramStart + (bitUsec >> 1);
    for (uint16_t i = 0; i < numBits; ++i, bitTime += bitUsec)
    {
        while (diagStopwatch.elapsed() < bitTime);
            ;
        param <<= 1;
        if (samplePort())
            param |= 1;
    }

    if (resync)
    {
        // Wait for the end of the stop bits and re-sync the clocks to correct any timing errors.
        uint32_t lastStopBitTime = paramStart + (numBits + 1) * bitUsec;
        while (diagStopwatch.elapsed() < lastStopBitTime)
            ;

        // TODO
    }

    return param;
}

bool ECU::readDiagPort(ECUSample& sample)
{
    if (!findPreamble())
        return false;

    const uint16_t wordLen = 11; // 1 start bit + 8 data bits + 2 stop bits
    const uint16_t wordUsec = wordLen * bitUsec;

    // TODO clean this up based on the PDF.  "16" and "5" are globbing together OBD-I ID and start bit.

    // First data word starts 16 bits after preamble goes low.
    const uint32_t InjTime      = (5 + wordLen) * bitUsec;
    const uint32_t IgnTime      = InjTime      + wordUsec;
    const uint32_t IACTime      = IgnTime      + wordUsec;
    const uint32_t RPMTime      = IACTime      + wordUsec;
    const uint32_t VAFTime      = RPMTime      + wordUsec;
    const uint32_t ECTTime      = VAFTime      + wordUsec;
    const uint32_t TPSTime      = ECTTime      + wordUsec;
    const uint32_t SpeedTime    = TPSTime      + wordUsec;
    const uint32_t Unknown1Time = SpeedTime    + wordUsec;
    const uint32_t Unknown2Time = Unknown1Time + wordUsec;
    const uint32_t Flag1Time    = Unknown2Time + wordUsec;
    const uint32_t Flag2Time    = Flag1Time    + wordUsec;

    // Read the serial parameters in succession
    uint16_t Unknown1, Unknown2, Flag1, Flag2;
    sample.injectorCentisec = readParameter(8, InjTime, true);
    sample.ignitionAngle = readParameter(8, IgnTime, true) - 90;
    sample.iac = readParameter(8, IACTime, true) * 100 / 125;
    sample.rpm = readParameter(8, RPMTime, true) * 25;
    sample.vaf = 125 - readParameter(8, VAFTime, true);
    sample.ect = readParameter(8, ECTTime, true);
    sample.tps = readParameter(8, TPSTime, true) / 2;
    sample.speed = readParameter(8, SpeedTime, true);
    Unknown1 = readParameter(8, Unknown1Time, true);
    Unknown2 = readParameter(8, Unknown2Time, true);
    Flag1 = readParameter(8, Flag1Time, true);
    Flag2 = readParameter(8, Flag2Time, false);

    return true;
}
