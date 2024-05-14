#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SI4735.h>

constexpr unsigned long long operator"" _Hz(unsigned long long hz) {
    return hz;
}

constexpr unsigned long long operator"" _kHz(unsigned long long hz) {
    return hz * 1000;
}

constexpr unsigned long long operator"" _MHz(unsigned long long hz) {
    return hz * 1000000;
}

struct Band
{
	int			mode;
	uint32_t	from;
	uint32_t	to;
	uint32_t	step;
	const char *name;
};

static Band BANDS[] = {
    {.mode = FM_CURRENT_MODE, .from = 87_MHz,    .to = 108_MHz,   .step = 50_kHz,  .name = "BROADCAST FM 87-108 MHZ"},
    {.mode = FM_CURRENT_MODE, .from = 64_MHz,    .to = 74_MHz,    .step = 10_kHz,  .name = "BROADCAST FM 64-74 MHZ"},
    {.mode = AM_CURRENT_MODE, .from = 153_kHz,   .to = 279_kHz,   .step = 1_kHz,   .name = "BROADCAST LW"},
	{.mode = AM_CURRENT_MODE, .from = 531_kHz,   .to = 2000_kHz,  .step = 1_kHz,   .name = "BROADCAST MW"},

	{.mode = AM_CURRENT_MODE, .from = 2300_kHz,  .to = 2495_kHz,  .step = 1_kHz,   .name = "BROADCAST 120M"},
	{.mode = AM_CURRENT_MODE, .from = 3200_kHz,  .to = 3400_kHz,  .step = 1_kHz,   .name = "BROADCAST 90M"},
	{.mode = AM_CURRENT_MODE, .from = 3900_kHz,  .to = 4000_kHz,  .step = 1_kHz,   .name = "BROADCAST 75M"},
	{.mode = AM_CURRENT_MODE, .from = 4750_kHz,  .to = 5060_kHz,  .step = 1_kHz,   .name = "BROADCAST 60M"},
	{.mode = AM_CURRENT_MODE, .from = 5900_kHz,  .to = 6200_kHz,  .step = 1_kHz,   .name = "BROADCAST 49M"},
	{.mode = AM_CURRENT_MODE, .from = 7200_kHz,  .to = 7600_kHz,  .step = 1_kHz,   .name = "BROADCAST 41M"},

	{.mode = AM_CURRENT_MODE, .from = 9400_kHz,  .to = 9900_kHz,  .step = 1_kHz,   .name = "BROADCAST 31M"},
	{.mode = AM_CURRENT_MODE, .from = 11600_kHz, .to = 12200_kHz, .step = 1_kHz,   .name = "BROADCAST 25M"},
	{.mode = AM_CURRENT_MODE, .from = 15100_kHz, .to = 15800_kHz, .step = 1_kHz,   .name = "BROADCAST 19M"},
	{.mode = AM_CURRENT_MODE, .from = 17480_kHz, .to = 17900_kHz, .step = 1_kHz,   .name = "BROADCAST 16M"},
	{.mode = AM_CURRENT_MODE, .from = 21450_kHz, .to = 21850_kHz, .step = 1_kHz,   .name = "BROADCAST 13M"},
    {.mode = AM_CURRENT_MODE, .from = 25600_kHz, .to = 26100_kHz, .step = 1_kHz,   .name = "BROADCAST 11M"},

	{.mode = AM_CURRENT_MODE, .from = 1800_kHz,  .to = 2000_kHz,  .step = 1_kHz,   .name = "AMAUTER 160M"},
	{.mode = AM_CURRENT_MODE, .from = 3500_kHz,  .to = 4000_kHz,  .step = 1_kHz,   .name = "AMAUTER 80M"},
	{.mode = AM_CURRENT_MODE, .from = 7000_kHz,  .to = 7300_kHz,  .step = 1_kHz,   .name = "AMAUTER 40M"},
	{.mode = AM_CURRENT_MODE, .from = 14000_kHz, .to = 14350_kHz, .step = 1_kHz,   .name = "AMAUTER 20M"},
	{.mode = AM_CURRENT_MODE, .from = 21000_kHz, .to = 21450_kHz, .step = 1_kHz,   .name = "AMAUTER 15M"},
	{.mode = AM_CURRENT_MODE, .from = 28000_kHz, .to = 28700_kHz, .step = 1_kHz,   .name = "AMAUTER 10M"},
};
static const int N_BANDS = sizeof(BANDS) / sizeof(BANDS[0]);

struct Receiver
{
	enum MODE {AM = 0, LSB, USB, FM};

	uint32_t	freq;
    uint32_t    step;
	MODE		mode;

    bool        pilot;
    uint8_t     snr;
    uint8_t     rssi;

    int         band_idx;	/* in BANDS array */
};

extern Receiver receiver;

void receiver_init();
void receiver_set_fm(uint32_t from, uint32_t to, uint32_t curr, uint32_t step);
void receiver_set_am(uint32_t from, uint32_t to, uint32_t curr, uint32_t step);
void receiver_set_frequency(uint32_t f);
void receiver_set_frequency_delta(int8_t delta);
void receiver_set_volume(uint8_t volume);
void receiver_sync_quality();
void receiver_mute();
void receiver_unmute();
void receiver_switch_to_band(int band_idx);