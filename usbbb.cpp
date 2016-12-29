#include <cassert>
#include "usbbb.hpp"

uint16_t sensordata::get_field(int x, int y) const
{
	assert(0 <= x && x < 10);
	assert(0 <= y && y < 10);
	if (x >= 1 && x <= 8 && y >= 1 && y <= 8) {
		return data[(x-1)*8+(8-y)];
	}
	return 0; // FIXME Rand-Sensoren noch nicht berücksichtigt.
}
