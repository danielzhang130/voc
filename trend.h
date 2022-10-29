#ifndef TREND_H
#define TREND_H

#include <Arduino.h>

class Trend {
public:
	Trend() : trend(), end(0) {}
	char get(uint16_t current);
private:
	uint16_t trend[6];
	uint8_t end;
};
#endif // !TREND_H
