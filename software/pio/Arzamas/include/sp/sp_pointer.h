/*-------------------------------------------------------------------------
 *
 * NOTES
 *  ******************************
 *  *** DO NOT EDIT THIS FILE! ***
 *  ******************************
 *
 *  It has been GENERATED 2024-04-24 20:09:46
 *
 *-------------------------------------------------------------------------
 */

#pragma once

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#include "sprite.h"

/* sp_pointer */
static const uint8_t sp_pointer_data[] PROGMEM = {
	0x03,
	0x02,
	0xFE,
	0x02,
	0x03,
	0x00,
};

static const Sprite sp_pointer = {
	.width	= 6,
	.height	= 8,
	.data	= sp_pointer_data,
};

