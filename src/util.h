#pragma once

#include <math.h>

static uint8_t color_to_byte(double c)
{
	return floor(c == 1.0 ? 255 : c * 256);
}

static double color_to_double(uint8_t c)
{
	return c / 255.0;
}
