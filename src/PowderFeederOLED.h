#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class PowderFeederOLED
{
    public:
        void setupLCD(int oledWidth, int oledHeight,
                        int oledI2CAddr);
        
        void clearLCD();
        void minimumAlertFlash();
        void maximumAlterFlash();

        void showRemoteIcon();
        void hideRemoteIcon();

    private:
        
        bool _isRemoteActive;
        bool _isSpinning;
        Adafruit_SSD1306 _oledScreenObj;
};