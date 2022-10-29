#ifndef DISPLAY_H
#define DISPLAY_H

#include "ER-EPM0154-1B.h"
#include "history_buffer.h"
#include <Arduino.h>

class Display {
public:
	Display();
	void update(uint16_t voc, char trend, char level, float volt, float percent);
	void history(HistoryBuffer history);
private:
	Epd epd;
	uint8_t x, y;
	boolean isHistory = false;
	void init();
	void off();
	void drawVoc(uint16_t voc);
	void drawTrend(char trend);
	void drawPpm();
	void drawLevel(char level);
	void drawBattery(float volt, float percent);
};
#endif // !DISPLAY_H
