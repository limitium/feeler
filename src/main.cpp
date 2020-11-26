#include "F_MHZ19.h"

Feeler::F_MHZ19 mzh19(16, 17);

void setup()
{
  Serial.begin(9600);
  mzh19.init();
}

void loop()
{
  mzh19.loop();
}