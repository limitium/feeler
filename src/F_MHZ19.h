#include "Feeler.h"
#include "MHZ19.h"

namespace Feeler
{
class F_MHZ19
{
private:
    MHZ19 *myMHZ19;
    Feeler *feeler;
    HardwareSerial *serial;

    unsigned long lastAdvert = 0;
    uint8_t loopCnt = 0;

public:
    F_MHZ19() {}
    F_MHZ19(int rxPin, int txPin)
    {
        this->feeler = new Feeler();
        this->serial = new HardwareSerial(1);
        this->serial->begin(9600, SERIAL_8N1, rxPin, txPin);
        this->myMHZ19 = new MHZ19();
    }
    void init()
    {
        feeler->init();
        myMHZ19->begin(*this->serial); // *Serial(Stream) refence must be passed to library begin().
        myMHZ19->autoCalibration(false);
    }
    void loop()
    {
        if (millis() - lastAdvert > 10000)
        {
            loopCnt++;
            lastAdvert = millis();
            if (loopCnt % 2 == 1)
            {
                int CO2;
                CO2 = myMHZ19->getCO2(true, true); // Request CO2 (as ppm)
                Serial.print("CO2 (ppm): ");
                Serial.println(CO2);

                feeler->advertCO2(CO2);
            }
            else
            {
                float temp = myMHZ19->getTemperature(true, true); // Request Temperature (as Celsius)
                Serial.print("Temperature (C): ");
                Serial.println(temp);

                feeler->advertT(temp * 10);
            }
        }
    }
};
} // namespace Feeler
