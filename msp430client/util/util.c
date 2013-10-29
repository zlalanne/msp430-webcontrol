
#include <stdint.h>

uint8_t UTIL_atoi(char *a, uint8_t aLength) {
	uint8_t value = 0;
	uint8_t i;

	for(i = 0; i < aLength; i++) {
		value = (value * 10) + (a[i] - '0');
	}

	return value;
}
