#include <Arduino.h>

#include "receiver.h"
#include "sp/sp_splash.h"
#include "sp/sp_fm_canvas.h"
#include "sp/sp_am_canvas.h"
#include "sp/sp_3d_venture_font.h"
#include "sp/sp_3x5_font.h"
#include "sp/sp_eh_font.h"
#include "sp/sp_pointer.h"
#include "sp/sp_stereo.h"
#include "sp/sp_mono.h"
#include "sp/sp_v_bar.h"
#include "sp/sp_band_select_canvas.h"
#include "sp/sp_band_select_pointer.h"
#include "ui.h"
#include "vfd.h"

static void menu_loop();
static Point menu_pointer_pos(int band_idx);
static Sprite char_to_sprite(char ch, Sprite sp, bool digital);
static void draw_receiver();
static void draw_bandmane();
static void draw_fm_freq(uint32_t freq);
static void draw_am_freq(uint32_t freq);
static void draw_fm_scale(uint32_t freq);
static void draw_am_scale(uint32_t freq);
static void draw_signal_quality();

void
setup()
{
	vfd_init();
	ui_init();
	receiver_init();
}

/*
 * Main event loop.
 */
void
loop()
{
	ui_sync();

	if (ui.enc1.changed) {
		receiver_set_frequency_delta(ui.enc1.curr - ui.enc1.prev);
		ui.enc1.changed = false;
	}

	if (ui.enc2.changed) {
		menu_loop();
		ui.enc2.changed = false;
	}

	draw_receiver();
}

/*
 * Handle band selection menu.
 */
static void
menu_loop()
{
	uint8_t	band_idx = receiver.band_idx;
	Point	p = menu_pointer_pos(band_idx);

	ui.enc2.changed = false;
	ui.enc2.pressed = false;

	while (1) {
		ui_sync();

		if (ui.enc2.pressed) {
			ui.enc2.pressed = false;
			break;
		}

		if (ui.enc2.changed) {
			int8_t delta = ui.enc2.curr - ui.enc2.prev;

			if (delta > 0) {
				if (band_idx < N_BANDS - 1)
					band_idx++;
			} else {
				if (band_idx > 0)
					band_idx--;
			}

			p = menu_pointer_pos(band_idx);
			ui.enc2.changed = false;
		}

		vfd_clb();
		vfd_draw({0, 0}, sp_band_select_canvas);
		vfd_draw(p, sp_band_select_pointer, Blend::ADD);
		vfd_sync();	
	}

	ui.enc2.changed = false;
	ui.enc2.pressed = false;

	if (receiver.band_idx != band_idx) {
		receiver_mute();
		receiver_switch_to_band(band_idx);
		draw_receiver();
		receiver_unmute();
	}
}

/*
 * Map band index to UI point.
 */
static Point
menu_pointer_pos(int band_idx)
{
	Point	p = {0, 0};

	if (band_idx == 0)
		p = {56, 15};
	else if (band_idx == 1)
		p = {56, 23};
	else if (band_idx == 2)
		p = {56, 31};
	else if (band_idx == 3)
		p = {56, 39};

	else if (band_idx == 4)
		p = {92, 15};
	else if (band_idx == 5)
		p = {92, 23};
	else if (band_idx == 6)
		p = {92, 31};
	else if (band_idx == 7)
		p = {92, 39};
	else if (band_idx == 8)
		p = {92, 47};
	else if (band_idx == 9)
		p = {92, 55};

	else if (band_idx == 10)
		p = {130, 15};
	else if (band_idx == 11)
		p = {130, 23};
	else if (band_idx == 12)
		p = {130, 31};
	else if (band_idx == 13)
		p = {130, 39};
	else if (band_idx == 14)
		p = {130, 47};
	else if (band_idx == 15)
		p = {130, 55};

	else if (band_idx == 16)
		p = {164, 15};
	else if (band_idx == 17)
		p = {164, 23};
	else if (band_idx == 18)
		p = {164, 31};
	else if (band_idx == 19)
		p = {164, 39};
	else if (band_idx == 20)
		p = {164, 47};
	else if (band_idx == 21)
		p = {164, 55};
	
	return p;
}

/*
 * Convert character into sprite. This support 10 and 95 charsets.
 */
static Sprite
char_to_sprite(char ch, Sprite sp, bool digital)
{
	const char 		ch_off = digital ? '0' : ' ';
	const uint16_t	ch_width = digital ? (sp.width / 10) : (sp.width / 95);
	const uint32_t	off = (ch - ch_off) * ch_width * (sp.height / 8 + 1);
	const uint8_t  *sp_upper = sp.data + off;

	return Sprite{
		.width	= ch_width,
		.height	= sp.height,
		.data	= sp_upper,
	};
}

/*
 * Display main scene.
 */
void
draw_receiver()
{
	vfd_clb();
	receiver_sync_quality();
	
	if (BANDS[receiver.band_idx].mode == FM_CURRENT_MODE) {
		vfd_draw({0, 0}, sp_fm_canvas);
		draw_fm_freq(receiver.freq);
		draw_fm_scale(receiver.freq);
	} else {
		vfd_draw({0, 0}, sp_am_canvas);
		draw_am_freq(receiver.freq);
		draw_am_scale(receiver.freq);
	}

	draw_bandmane();
	draw_signal_quality();
	vfd_sync();
}

/*
 * Draw n-th char from buffer with base point.
 */
static void
draw_freq_char(const char *str, int n, Point base, Sprite font = sp_3d_venture_font,
			   bool digital = true)
{
	Sprite			sp = char_to_sprite(str[n], font, digital);
	uint16_t		x = base.x + n * sp.width;

	vfd_draw({x, base.y}, sp, Blend::ADD);
}

void
draw_bandmane()
{
	int 		n = strlen(BANDS[receiver.band_idx].name);
	uint16_t	w = 148 - (sp_eh_font.width / 95) * (n / 2);

	for (int i = 0; i < n; i++)
		draw_freq_char(BANDS[receiver.band_idx].name, i, {w, 2}, sp_eh_font, false);
}

/*
 * Print FM freq as DDD DD.
 */
static void
draw_fm_freq(uint32_t freq)
{
	char		buf[8];
	uint32_t	upper,
				lower;
	
	freq /=  1_kHz;
	upper = (freq / 1000) % 1000;
	lower = freq % 1000 / 10;
	
	snprintf(buf, sizeof(buf), "%03lu", upper);

	for (int i = buf[0] == '0' ? 1 : 0; i < 3; i++)
		draw_freq_char(buf, i, {96, 39});

	snprintf(buf, sizeof(buf), "%02lu", lower);

	for (int i = 0; i < 2; i++)
		draw_freq_char(buf, i, {134, 39});
}

/*
 * Print AM freq as DDD DD.
 */
static void
draw_am_freq(uint32_t freq)
{
	char	buf[8];
	int		i;

	freq /= 1_kHz;
	snprintf(buf, sizeof(buf), "%05lu", freq);

	i = buf[1] == '0' ? 2 : (buf[0] == '0' ? 1 : 0);
	for (; i < 5; i++)
		draw_freq_char(buf, i, {96, 39});
}

static void
draw_fm_scale(uint32_t freq)
{
	constexpr int 	left = 52;
	constexpr int 	right = 244;
	uint16_t 		x = map(freq / 100_kHz,
							BANDS[receiver.band_idx].from / 100_kHz,
							BANDS[receiver.band_idx].to / 100_kHz,
							left, right);

	vfd_draw({x, 18}, sp_pointer, Blend::ADD);

	char		buf[8];
	int 		n;

	n = snprintf(buf, sizeof(buf), "%u",
				 (unsigned) (BANDS[receiver.band_idx].from / 1_MHz));

	for (int i = 0; i < n; i++)
		draw_freq_char(buf, i, {54, 27}, sp_3x5_font, false);
	
	n = snprintf(buf, sizeof(buf), "%u",
				 (unsigned) (BANDS[receiver.band_idx].to / 1_MHz));
	x = 247 - sp_3x5_font.width / 95 * n;

	for (int i = 0; i < n; i++)
		draw_freq_char(buf, i, {x, 27}, sp_3x5_font, false);
}

static void
draw_am_scale(uint32_t freq)
{
	constexpr int 	left = 52;
	constexpr int 	right = 244;
	uint16_t 		x = map(freq / 1_kHz,
							BANDS[receiver.band_idx].from / 1_kHz,
							BANDS[receiver.band_idx].to / 1_kHz,
							left, right);

	vfd_draw({x, 18}, sp_pointer, Blend::ADD);

	char		buf[8];
	int 		n;

	n = snprintf(buf, sizeof(buf), "%u", (unsigned) (BANDS[receiver.band_idx].from / 1_kHz));
	for (int i = 0; i < n; i++)
		draw_freq_char(buf, i, {54, 27}, sp_3x5_font, false);
	
	n = snprintf(buf, sizeof(buf), "%u", (unsigned) (BANDS[receiver.band_idx].to / 1_kHz));
	x = 247 - sp_3x5_font.width / 95 * n;
	for (int i = 0; i < n; i++)
		draw_freq_char(buf, i, {x, 27}, sp_3x5_font, false);
}

static void
draw_signal_quality()
{
	vfd_draw({161, 39}, receiver.pilot ? sp_stereo : sp_mono, Blend::ADD);

	uint16_t    x = 11;
    uint8_t		off = 1 + 54;
	uint8_t		snr = map(receiver.snr, 0, 32, 1, 8);
	uint8_t		rssi = map(receiver.snr, 0, 32, 1, 8);

	for (int i = 0; i < snr; ++i)
		vfd_draw({x, off -= sp_v_bar.height}, sp_v_bar, Blend::ADD);
	
	off = 1 + 54;
	x += sp_v_bar.width + 2;
	for (int i = 1; i < rssi; ++i)
		vfd_draw({x, off -= sp_v_bar.height}, sp_v_bar, Blend::ADD);
}