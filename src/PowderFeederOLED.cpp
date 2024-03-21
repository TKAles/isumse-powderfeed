#include <PowderFeederOLED.h>

void PowderFeederOLED::setupLCD(int oledWidth, int oledHeight, int oledI2Caddr)
{
    this->_oledScreenObj = Adafruit_SSD1306(oledWidth, oledHeight, &Wire);
    this->_oledScreenObj.begin(SSD1306_SWITCHCAPVCC, oledI2Caddr);
    this->clearLCD();
    this->_oledScreenObj.invertDisplay(true);
    sleep(125);
    this->_oledScreenObj.invertDisplay(false);
}

void PowderFeederOLED::clearLCD()
{
    this->_oledScreenObj.clearDisplay();
    this->_oledScreenObj.display();
    return;
}