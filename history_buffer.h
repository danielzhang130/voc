#ifndef HISTORY_BUFFER_H
#define HISTORY_BUFFER_H

#include <Arduino.h>
#include <math.h>
#undef min
#undef max

constexpr uint8_t BUFFER_SIZE = 150;
constexpr uint8_t SKIP_SIZE = 29;

class HistoryBuffer {
public:

	struct Info {
		uint16_t min;
		uint16_t max;
	};

	HistoryBuffer() : buffer(), end(0), skip(SKIP_SIZE) {
		for (uint8_t i = 0; i < BUFFER_SIZE; ++i) {
			buffer[i] = 1000 >> 5;
			//buffer[i] = ((int) (200 * cos(i / (2 * PI)) + 1000)) >> 5;
		}
	}

	void write(uint16_t num) {
		sum += num;
		if (0 != skip) {
			--skip;
			return;
		}

		skip = SKIP_SIZE;
		uint16_t average = sum / (SKIP_SIZE + 1);
		buffer[end++] = (average & 0x1FFF) >> 5 ;

		if (end == BUFFER_SIZE) {
			end = 0;
		}
		sum = 0;
	}

	uint8_t size() {
		return BUFFER_SIZE;
	}

	uint16_t get(uint8_t index) {
		index += end;

		if (index >= BUFFER_SIZE) {
			index -= BUFFER_SIZE;
		}

		return buffer[index] << 5;
	}

	Info info() {
		Info info = {
			buffer[0], buffer[0]
		};

		for (uint8_t i = 1; i < BUFFER_SIZE; ++i) {
			if (buffer[i] == 0) {
				continue;
			}
			if (buffer[i] < info.min) {
				info.min = buffer[i];
			}
			if (buffer[i] > info.max) {
				info.max = buffer[i];
			}
		}

		info.min = info.min << 5;
		info.max = info.max << 5;
		return info;
	}
private:
	uint8_t buffer[BUFFER_SIZE];

	uint8_t end;
	uint8_t skip;

	uint32_t sum;
};

#endif // !HISTORY_BUFFER_H
