#include <iostream>
#include <exception>
#include "usbbb.hpp"

const int field_size_x = 10;
const int field_size_y = 10;

struct particle {
	float level;
	float speed;
};

struct particle_field {
	particle field[field_size_x * field_size_y];

	particle_field();

	particle const& operator()(int x, int y) const {
		return field[y * field_size_x + x];
	}

	particle & operator()(int x, int y) {
		return field[y * field_size_x + x];
	}
};

particle_field::particle_field()
{
	particle zero = { 0.f, 0.f };
	std::fill(std::begin(field), std::end(field), zero);
}

float get_level(particle_field const& pf, int x, int y) {
	return (x < 0 || x >= 10 || y < 0 || y >= 10) ? 0.f : pf(x,y).level;
}

void update_level(particle_field & pf) {
	for (int y = 0; y < field_size_y; ++y) {
		for (int x = 0; x < field_size_x; ++x) {
			auto& p = pf(x,y);
			p.level += p.speed;
		}
	}
}

void update_speed(particle_field & pf) {
	for (int y = 0; y < field_size_y; ++y) {
		for (int x = 0; x < field_size_x; ++x) {
			auto& p = pf(x,y);
			auto m = ( get_level(pf,x-1,y-1) +
			           get_level(pf,x+1,y-1) +
			           get_level(pf,x-1,y+1) +
			           get_level(pf,x+1,y+1) ) * (0.7f/6.8f) +
			         ( get_level(pf,x,y-1) +
			           get_level(pf,x,y+1) +
			           get_level(pf,x-1,y) +
			           get_level(pf,x+1,y) ) * (1.0f/6.8f);
			auto delta = m - p.level;
			p.speed += delta * 0.03f;
			p.speed *= 0.99f;
		}
	}
}

int clamp8(int x) {
	return x < 0 ? 0 : x > 255 ? 255 : x;
}

bool border(int x, int y) {
	return x<=0 || x>=field_size_x-1 || y<=0 || y>=field_size_y-1;
}

int main() {
	try {
		bbxx blinkbox;
		sensordata sd;
		particle_field phystate;
		for (int time = 0; ; time = (time + 1) % 500) {
			for (int y = 0; y < field_size_y; ++y) {
				for (int x = 0; x < field_size_x; ++x) {
					auto s = sd.get_field(x,y);
					if (s > 3000) {
						auto& p = phystate(x,y);
						p.speed += 3;
					}
				}
			}
			update_level(phystate);
			update_speed(phystate);
			for (int y = 0; y < field_size_y; ++y) {
				for (int x = 0; x < field_size_x; ++x) {
					int qr = clamp8( 96 + phystate(x,y).level);
					int qg = clamp8(128 + phystate(x,y).level);
					int qb = clamp8(160 + phystate(x,y).level);
					if (!border(x, y)) {
						qr /= 2;
						qg /= 2;
						qb /= 2;
					}
					blinkbox.set_led10(x, y, qr, qg, qb);
				}
			}
			blinkbox.transmit(-1);
			blinkbox.wait_measure();
			blinkbox.get_sensordata(sd.data);
		}
		return EXIT_SUCCESS;
	} catch (std::exception const& x) {
		std::cerr << "Da hat leider etwas nicht geklappt:\n" << x.what() << '\n';
		return EXIT_FAILURE;
	}
}
