#include <math.h>

static uint8_t convert_color(double c)
{
	return floor(c == 1.0 ? 255 : c * 256);
}
