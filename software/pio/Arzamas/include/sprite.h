#pragma once

#include <stdint.h>

struct Sprite
{
	uint16_t		width;	/* in px */
	uint8_t			height;	/* in px */
	const uint8_t  *data;
};