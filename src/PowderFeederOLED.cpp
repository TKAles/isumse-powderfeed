#include <PowderFeederOLED.h>

void PowderFeederOLED::setupLCD(int oledWidth, int oledHeight, int oledI2Caddr)
{
    this->_oledScreenObj = Adafruit_SSD1306(oledWidth, oledHeight, &Wire);
    this->_oledScreenObj.begin(SSD1306_SWITCHCAPVCC, oledI2Caddr);
    this->clearLCD();
    delay(250);
    this->_oledScreenObj.invertDisplay(false);
    this->_oledScreenObj.display();

}

void PowderFeederOLED::flashInvertLCD(int _duration)
{
    this->_oledScreenObj.invertDisplay(true);
    delay(_duration);
    this->_oledScreenObj.invertDisplay(false);
}
void PowderFeederOLED::clearLCD()
{
    this->_oledScreenObj.clearDisplay();
    this->_oledScreenObj.display();
    return;
}
void PowderFeederOLED::drawRPMScreen(int _RPMScaledByOneTen)
{
    const char _rpmstr[] = "RPM:";
    // Clear the display first.
    this->_oledScreenObj.clearDisplay();
    this->_oledScreenObj.setTextColor(SSD1306_WHITE);
    // Add the 'RPM' text to the top left at size 2
    this->_oledScreenObj.setTextSize(2);
    this->_oledScreenObj.setCursor(0,0);
    this->_oledScreenObj.print(_rpmstr);
    char _rpmval[4];
    float _rpmFloat = _RPMScaledByOneTen / 10.0;
    dtostrf(_rpmFloat, 4, 1, _rpmval);
    this->_oledScreenObj.setTextSize(4);
    this->_oledScreenObj.setCursor(0, 28);
    this->_oledScreenObj.print(_rpmval);
    this->_oledScreenObj.display();
}