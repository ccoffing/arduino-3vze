#include "Adafruit_GFX.h"
//#include "Fonts/FreeMono12pt7b.h"
#include "Adafruit_ILI9340.h"
#include "SPI.h"

#include "stopwatch.h"
#include "ecu.h"

// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk  13
#define _miso  12
#define _mosi  11
#define TFT_CS_PIN    10
#define TFT_DS_PIN     9
#define TFT_RST_PIN    8


static Adafruit_ILI9340 tft(TFT_CS_PIN, TFT_DS_PIN, TFT_RST_PIN);


class Tab
{
public:
    virtual void show()
    {
        tft.fillScreen(ILI9340_BLACK);
    }

    virtual void hide()
    {
    }

    virtual void loop() = 0;
};

class ECUTab : public Tab
{
public:
    ECUTab() :
        m_waitState(0)
    {
    }

    void loop()
    {
        bool updated = m_ecu.readDiagPort(m_sample);

        if (updated)
        {
            if (m_waitState)
            {
                m_waitState = 0;
                tft.fillScreen(ILI9340_BLACK);
            }

            char buf[16];

            //tft.setFont(&FreeMono12pt7b);
            tft.setTextColor(ILI9340_WHITE);
            tft.setTextSize(2);

            sprintf(buf, "%3u RPM", m_sample.rpm);
            tft.setCursor(0, 20);
            tft.print(buf);

            sprintf(buf, "%3u MPH", m_sample.rpm);
            tft.setCursor(100, 20);
            tft.print(buf);

            sprintf(buf, "%3u ECT", m_sample.ect);
            tft.setCursor(200, 20);
            tft.print(buf);
        }
        else
        {
            tft.setTextColor(ILI9340_WHITE);
            tft.setTextSize(3);
            if (m_waitState == 0)
            {
                tft.fillRect(0, 80, 320, 80, ILI9340_BLUE);
                tft.setCursor(20, 100);
                tft.println("Waiting for ECU");
            }
            tft.setCursor(0, 120);
            tft.setTextColor(ILI9340_BLUE);
            tft.print("                ."  + (m_waitState % 17));
            m_waitState++;
            tft.setCursor(0, 120);
            tft.setTextColor(ILI9340_WHITE);
            tft.print("                ."  + (m_waitState % 17));
        }
    }

    ECU m_ecu;
    ECUSample m_sample;
    uint8_t m_waitState;
};

class Clock : public Tab
{
public:
    void loop()
    {
        unsigned long ms = millis();
        if (ms - prevMs < 1000)
            return;
        prevMs = ms;

        String s(ms);
        tft.setTextSize(2);

        tft.setCursor(0, 100);
        tft.setTextColor(ILI9340_WHITE, ILI9340_BLACK);
        tft.println(s);
    }

    unsigned long prevMs;
};

class Switcher
{
public:
    Switcher()
    {
        tabs[0] = new Clock;
        tabs[1] = new ECUTab;
    }

    void loop()
    {
        tabs[tab]->loop();
    }

    void select(int t)
    {
        tab = t;
        tabs[tab]->show();
    }

    // buttons

    Tab* tabs[3];
    int tab = 0;
} switcher;

void setup()
{
    Serial.begin(9600);

    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(ILI9340_BLACK);

    pinMode(ECU::pin, INPUT);

    switcher.select(1);
}

void loop()
{
    switcher.loop();
}
