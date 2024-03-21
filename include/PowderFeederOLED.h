#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class PowderFeederOLED
{
    public:
        void setupLCD(int oledWidth, int oledHeight,
                        int oledI2CAddr);
        
        void clearLCD();
        void ClippingAlertFlash(bool _minMode = true);

        void showRemoteIcon();
        void hideRemoteIcon();

        void flashInvertLCD(int _duration=100);
        void drawRPMScreen(int _RPMScaledByTen);

    private:
        
        bool _isRemoteActive;
        bool _isSpinning;
        Adafruit_SSD1306 _oledScreenObj;
};