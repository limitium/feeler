#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEBeacon.h>
#include <sstream>
#include <iomanip>
#include <esp_bt_device.h>

namespace Feeler
{

class Feeler
{

private:
    BLEAdvertising *pAdvertising;
    const uint8_t *bdAddr;
    uint8_t frame = 0;

    unsigned int hexToUint(const char *str)
    {
        std::istringstream converter(str);
        unsigned int value;
        converter >> std::hex >> value;
        return value;
    }

public:
    void init()
    {

        BLEDevice::init("Feeler");
        pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
        pAdvertising->setMinPreferred(0x12);

        bdAddr = esp_bt_dev_get_address();
    }
    void setAdvertData(BLEAdvertisementData &adv, int frame, uint16_t dataType, uint16_t dataValue)
    {
        BLEUUID uuid = BLEUUID::fromString("0xfe95");
        adv.setFlags(0x06 | 0x04);
        adv.setCompleteServices(uuid);

        // UUID | 0 index Frame control | Sensor type | Frame count | MAC          | Capability | Data type | Len | Val
        // 95fe | 7120                  | fffe        | bc          | fe986b8d7cc4 | 0d         | fffe      | 03  | 000000
        //      | 113 32                | 255 254     | 0           | 1 2 3 4 5 6  | 13         | 255 254   | 1   | 1

        uint8_t headerSize = 15;
        uint8_t dataSize = 2;
        uint8_t headerData[headerSize + dataSize];

        headerData[0] = hexToUint("71");
        headerData[1] = hexToUint("20");

        headerData[2] = hexToUint("ff");
        headerData[3] = hexToUint("fe");

        headerData[4] = frame;

        headerData[5] = bdAddr[5];
        headerData[6] = bdAddr[4];
        headerData[7] = bdAddr[3];
        headerData[8] = bdAddr[2];
        headerData[9] = bdAddr[1];
        headerData[10] = bdAddr[0];

        headerData[11] = hexToUint("0d");

        headerData[12] = dataType >> 8 & 0xff;
        headerData[13] = dataType & 0xff;

        headerData[14] = dataSize;

        headerData[15] = dataValue & 0xff;
        headerData[16] = dataValue >> 8 & 0xff;

        std::string data(reinterpret_cast<const char *>(&headerData), headerSize + dataSize);
        adv.setServiceData(uuid, data);
    }

    void advert(uint16_t dataType, uint16_t dataValue)
    {
        frame++;

        BLEAdvertisementData adv;
        setAdvertData(adv, frame, dataType, dataValue);

        pAdvertising->setAdvertisementData(adv);
        pAdvertising->start();
        delay(100);
        pAdvertising->stop();
    }
    void advertCO2(uint16_t dataValue)
    {
        advert(hexToUint("ff") << 8 | hexToUint("fe"), dataValue);
    }
    void advertT(uint16_t dataValue)
    {
        advert(hexToUint("04") << 8 | hexToUint("10"), dataValue);
    }
};
}; // namespace Feeler