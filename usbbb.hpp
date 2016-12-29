#ifndef USBBB_HPP_INCLUDED
#define USBBB_HPP_INCLUDED

#include <cassert>
#include <stdexcept>
#include <algorithm>
#include "usbbb.h"

struct position { int x; int y; };

struct sensordata {
	uint16_t data[12*8];

	uint16_t get_field(int x, int y) const;
};

class bbxx
{
public:
	bbxx() : rawptr(nullptr) { if (bb_open(&rawptr)) throw std::runtime_error("BB context creation error"); }
	bbxx(bbxx && rref) : rawptr(rref.rawptr) { rref.rawptr = nullptr; }
	~bbxx() { if (rawptr) bb_free(rawptr); }
	bbxx& operator=(bbxx temp) { std::swap(temp.rawptr, this->rawptr); return *this; }

	bb_ctx const* get() const { return rawptr; }
	bb_ctx* get() { return rawptr; }

	position get_ledpos(int ledno) const
	{
		ensure_valid();
		position result;
		bb_get_led_pos(rawptr, ledno, &result.x, &result.y);
		return result;
	}

	void set_led(int ledno, uint8_t r, uint8_t g, uint8_t b)
	{ ensure_valid(); bb_set_led(rawptr, ledno, r, g, b); }

	void set_led10(int x, int y, int r, int g, int b)
	{ ensure_valid(); bb_set_led10(rawptr, x, y, r, g, b); }

	void set_led40(int x, int y, int r, int g, int b)
	{ ensure_valid(); bb_set_led40(rawptr, x, y, r, g, b); }

	void transmit(int measure_row)
	{ ensure_valid(); bb_transmit(rawptr, measure_row); }

	int wait_measure()
	{ ensure_valid(); return bb_wait_measure(rawptr); }

	void get_sensordata(uint16_t sensordata[])
	{ ensure_valid(); bb_get_sensordata(rawptr, sensordata); }

	bbxx(bbxx const&) = delete; // not copyable, just movable

private:
	bb_ctx* rawptr;

	void ensure_valid() const { if (!rawptr) throw std::logic_error("Invalid BB context"); }
};

#endif

