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

/* sp_v_bar */
static const uint8_t sp_v_bar_data[] PROGMEM = {
	0xFC,
	0xA8,
	0x54,
	0xA8,
	0xFC,
};

static const Sprite sp_v_bar = {
	.width	= 5,
	.height	= 6,
	.data	= sp_v_bar_data,
};

