#include "display.h"
#include "digits.h"
#include "imagedata.h"
#include "epdpaint.h"
#include "arrow.h"
#include "bars.h"


constexpr auto FONT_WIDTH = 32;
constexpr auto FONT_HEIGHT = 53;
constexpr auto FONT_SIZE = FONT_WIDTH * FONT_HEIGHT / 8;

constexpr auto DISPLAY_WIDTH = 152;
constexpr auto DISPLAY_HEIGHT = DISPLAY_WIDTH;

constexpr auto COLORED = 0;
constexpr auto UNCOLORED = 1;

Display::Display()
{
}

void Display::update(uint16_t voc, char trend, char level, float volt, float percent)
{
    isHistory = false;
    init();
    epd.ClearFrame();

    x = (DISPLAY_WIDTH - 4 * FONT_WIDTH) / 2;
    y = (DISPLAY_HEIGHT - FONT_HEIGHT) / 2;

    drawVoc(voc);
    drawTrend(trend);
    drawPpm();
    drawLevel(level);
    drawBattery(volt, percent);

    epd.DisplayFrame();

    off();
}

void Display::history(HistoryBuffer history)
{
    if (isHistory) {
        return;
    }
    isHistory = true;

    init();
    epd.ClearFrame();

    x = 0;
    y = 16;

    auto info = history.info();
    auto min = info.min;
    auto max = info.max;
    auto diff = max - min;
    uint8_t res = diff / (DISPLAY_HEIGHT - 32) + 1;

    //unsigned char image;
    //Paint paint(&image, 8, 1);

    //for (; x < 152; x += 8) {
    //    for (y = 16; y <= DISPLAY_HEIGHT - 16; ++y) {
    //        paint.Clear(UNCOLORED);

    //        for (uint8_t j = x; j < x + 8 && j < 150; ++j) {
    //            uint16_t value = history.get(j);
    //            uint8_t height = DISPLAY_HEIGHT - 16 - (value - min) / res;
    //            if (height == y) {
    //                paint.DrawPixel((j - x), 0, COLORED);
    //                if (j - x == 7) {
    //                    paint.DrawPixel((j - x) - 1, 0, COLORED);
    //                }
    //                else {
    //                    paint.DrawPixel((j - x) + 1, 0, COLORED);
    //                }
    //            }
    //        }
    //        epd.SetPartialWindow(paint.GetImage(), x, y, 8, 1);
    //    }
    //}

    unsigned char image[DISPLAY_HEIGHT - 32]{};
    Paint paint(image, 8, DISPLAY_HEIGHT - 32);

    paint.Clear(UNCOLORED);

    for (uint8_t i = 0; i < history.size(); ++i) {
        uint16_t value = history.get(i);

        if (value != 0) {
            paint.DrawPixel((2 + i) % 8, DISPLAY_HEIGHT - 32 - (value - min) / res, COLORED);
            paint.DrawPixel((2 + i) % 8, DISPLAY_HEIGHT - 32 - (value - min) / res - 1, COLORED);
        }

        if (i % 8 == 5) {
            epd.SetPartialWindow(paint.GetImage(), x + i, y, paint.GetWidth(), paint.GetHeight());
            paint.Clear(UNCOLORED);
        }
    }

    char str[5];
    sprintf(str, "%d", min);

    unsigned char image2[400]{};
    Paint paint2(image2, 48, 16);

    paint2.Clear(UNCOLORED);
    paint2.DrawStringAt(0, 0, str, &Font16, COLORED);
    epd.SetPartialWindow(paint2.GetImage(), 2, DISPLAY_HEIGHT - 16, paint2.GetWidth(), paint2.GetHeight());
    
    paint2.Clear(UNCOLORED);
    sprintf(str, "%d", max);
    paint2.DrawStringAt(0, 0, str, &Font16, COLORED);
    epd.SetPartialWindow(paint2.GetImage(), 2, 2, paint2.GetWidth(), paint2.GetHeight());

    paint2.SetWidth(8);
    paint2.Clear(UNCOLORED);
    for (uint8_t i = 0; i < 15; ++i) {
        paint2.DrawPixel(0, i + 1, COLORED);
    }
    for (uint8_t i = 0; i < 5; ++i) {
        epd.SetPartialWindow(paint2.GetImage(), DISPLAY_WIDTH - 24 * i, DISPLAY_HEIGHT - 16, 8, 16);
    }

    epd.DisplayFrame();

    off();
}

void Display::init()
{
    pinMode(13, OUTPUT);
    
    Serial.println("e-Paper initializing");
    if (epd.Init() != 0) {
        Serial.println("e-Paper init failed");
        return;
    }

    Serial.println("e-Paper init go");

    epd.SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    epd.SendData(0x77);
}

void Display::off()
{
    epd.Sleep();
    pinMode(13, INPUT);
}

void Display::drawVoc(uint16_t voc)
{
    unsigned char image[FONT_SIZE]{};

    char num[4]{};
    for (uint8_t i = 0; i < 4; ++i) {
        num[3 - i] = voc % 10;
        voc /= 10;
    }

    uint8_t i = 0;
    while (num[i] == 0) {
        ++i;
        x += FONT_WIDTH;
    }

    for (; i < 4; ++i) {
        char c = num[i];
        for (uint8_t j = 0; j < FONT_SIZE; ++j) {
            image[j] = pgm_read_byte(digits[c] + j);
        }
        epd.SetPartialWindow(image, x, y, FONT_WIDTH, FONT_HEIGHT);
        x += FONT_WIDTH;
    }
}

void Display::drawTrend(char trend)
{
    if (trend == 0) {
        return;
    }

    uint8_t image[32]{};

    y += FONT_HEIGHT / 2;

    if (trend > 0) {
        y -= 16;
        for (uint8_t j = 0; j < 32; ++j) {
            image[j] = pgm_read_byte(arrows[0] + j);
        }
    }
    else if (trend < 0) {
        for (uint8_t j = 0; j < 32; ++j) {
            image[j] = pgm_read_byte(arrows[1] + j);
        }
    }
    epd.SetPartialWindow(image, x, y, 16, 16);
}

void Display::drawPpm()
{
    x = 80;
    y = 105;
    uint8_t image[600]{};
    Paint paint(image, 152, 30);

    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 0, "CO ppm", &Font16, COLORED);
    paint.DrawStringAt(22, 8, "2", &Font12, COLORED);
    epd.SetPartialWindow(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());
}

void Display::drawLevel(char level)
{
    x = 0;
    y = DISPLAY_HEIGHT - 16;

    uint8_t image[304]{};

    for (uint16_t j = 0; j < 304; ++j) {
        image[j] = pgm_read_byte(bars[level] + j);
    }
    epd.SetPartialWindow(image, x, y, 152, 16);
}

void Display::drawBattery(float volt, float percent)
{
    x = 0;
    y = 2;

    unsigned char image[400]{};
    Paint paint(image, 152, 16);

    paint.Clear(UNCOLORED);
    paint.DrawRectangle(2, 0, 35, 15, COLORED);
    paint.DrawFilledRectangle(3, 1, (35-2)*percent, 14, COLORED);
    paint.DrawFilledRectangle(35, 3, 38, 11, COLORED);

    char p_string[4];
    sprintf(p_string, "%d%%", (uint8_t)(percent * 100));
    paint.DrawStringAt(42, 2, p_string, &Font16, COLORED);

    char v_string[5];
    sprintf(v_string, "%dV", (uint16_t)(volt * 100));
    Serial.println(v_string);
    paint.DrawStringAt(152 - 4 * 11, 2, v_string, &Font16, COLORED);
    paint.DrawPixel(152 - 4 * 11 + 10, 12, COLORED);
    paint.DrawPixel(152 - 4 * 11 + 11, 12, COLORED);

    epd.SetPartialWindow(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());
}
