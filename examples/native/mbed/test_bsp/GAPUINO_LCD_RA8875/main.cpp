#include "mbed.h"
#include "RA8875.h"

RA8875 lcd(SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CSN0_A5, GPIO_A5_B40, "tft");    // MOSI, MISO, SCK, /ChipSelect, /reset, name

int main()
{
    lcd.init();      // defaults to 480x272. use parameters for 800x480
    lcd.printf("printing 3 x 2 = %d", 3*2);
    lcd.circle(       400,25,  25,               BrightRed);
    lcd.fillcircle(   400,25,  15,               RGB(128,255,128));
    lcd.ellipse(      440,75,  35,20,            BrightBlue);
    lcd.fillellipse(  440,75,  25,10,            Blue);
    lcd.triangle(     440,100, 475,110, 450,125, Magenta);
    lcd.filltriangle( 445,105, 467,111, 452,120, Cyan);
    lcd.rect(         400,130, 475,155,          Brown);
    lcd.fillrect(     405,135, 470,150,          Pink);
    lcd.roundrect(    410,160, 475,190, 10,8,    Yellow);
    lcd.fillroundrect(415,165, 470,185,  5,3,    Orange);
    lcd.line(         430,200, 460,230,          RGB(0,255,0));
    for (int i=0; i<=30; i+=5)
        lcd.pixel(435+i,200+i, White);
}
