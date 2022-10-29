#include "trend.h"
#include <math.h>

char Trend::get(uint16_t current)
{
	trend[end++] = current;

	if (end == 6) {
		end = 0;
	}

	//Serial.print("end: ");
	//Serial.println(end);

	short sum1 = trend[end];
	short sum2 = 0;

	//Serial.print("start = ");
	//Serial.println(trend[end]);

	uint8_t i = end + 1;
	if (i == 6) {
		i = 0;
	}

	uint8_t count = 1;

	while (i != end) {
		//Serial.print("i = ");
		//Serial.println(i);
		//Serial.println(trend[i]);

		if (count++ < 3) {
			sum1 += trend[i];
		}
		else {
			sum2 += trend[i];
		}

		++i;
		if (i == 6) {
			i = 0;
		}
	}

	sum1 /= 3;
	sum2 /= 3;

	//Serial.println(sum1);
	//Serial.println(sum2);

	if (abs(sum1 - sum2) < 50) {
		return 0;
	}
	if (sum2 > sum1) {
		return 1;
	}
	return -1;
}
