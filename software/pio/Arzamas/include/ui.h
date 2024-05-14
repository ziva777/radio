#pragma once

#include <Arduino.h>

struct Enc
{
	uint16_t	curr;
	uint16_t	prev;
	bool		pressed;
	bool		changed;
};

struct Switch
{
	bool	curr;
	bool	prev;
};

struct UI
{
	Enc			enc1;
	Enc			enc2;
	Switch		sw1;
	Switch		sw2;
};

extern UI ui;

void ui_init();
void ui_sync();