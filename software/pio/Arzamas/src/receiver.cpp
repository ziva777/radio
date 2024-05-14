#include "receiver.h"

#include "wiring.h"

constexpr int RESET_PIN = 8;
constexpr int FM_FUNCTION = 0;
constexpr int AM_FUNCTION = 1;

static SI4735   rx;
static uint32_t last_freq[N_BANDS];	/* last freq cache */
Receiver        receiver;

void
receiver_init()
{
    rx.setVolume(0);

    digitalWrite(RESET_PIN, HIGH);
    if (rx.getDeviceI2CAddress(RESET_PIN) == 0)
        while (1);

    // pinMode(AMP_MUTE_ARDUINO_PIN, OUTPUT);
    // digitalWrite(AMP_MUTE_ARDUINO_PIN, HIGH);
    
    // pinMode(AMP_MODE_ARDUINO_PIN, OUTPUT);
    // digitalWrite(AMP_MODE_ARDUINO_PIN, HIGH);

    receiver.band_idx = 0;
    receiver.freq = 89_MHz + 500_kHz;
    last_freq[receiver.band_idx] = receiver.freq;

    receiver_switch_to_band(receiver.band_idx);
    receiver_unmute();
}

void
receiver_set_fm(uint32_t from, uint32_t to, uint32_t curr, uint32_t step)
{
    rx.setup(RESET_PIN, FM_FUNCTION);
    rx.setFM(from / 10000, to / 10000, curr / 10000, step / 10000);
    rx.setAutomaticGainControl(0, 0);
}

void
receiver_set_am(uint32_t from, uint32_t to, uint32_t curr, uint32_t step)
{
    rx.setup(RESET_PIN, AM_FUNCTION);
    rx.setAM(from / 10000, to / 10000, curr / 10000, step / 10000);
    rx.setAutomaticGainControl(0, 0);
}

void
receiver_set_frequency(uint32_t f)
{
    static uint32_t cache;

    if (f != cache) {
        rx.setFrequency(f / 10000);
        cache = f;
    }
}

void
receiver_set_frequency_delta(int8_t delta)
{
    receiver.freq += delta * receiver.step;
    if (receiver.freq < BANDS[receiver.band_idx].from)
        receiver.freq = BANDS[receiver.band_idx].from;
    else if (receiver.freq > BANDS[receiver.band_idx].to)
        receiver.freq = BANDS[receiver.band_idx].to;

    receiver_set_frequency(receiver.freq);
}

void
receiver_set_volume(uint8_t volume)
{
    rx.setVolume(volume);
}

void
receiver_sync_quality()
{
    rx.getCurrentReceivedSignalQuality();

    receiver.pilot = rx.getCurrentPilot();
    receiver.snr = rx.getCurrentSNR();
    receiver.rssi = rx.getCurrentRSSI();
}

void
receiver_mute()
{
    for (int i = 63; i >= 0; i--) {
        receiver_set_volume(i);
        delay(10);
    }
}

void
receiver_unmute()
{
    for (int i = 0; i < 64; i++) {
		receiver_set_volume(i);
		delay(10);
	}
}

void
receiver_switch_to_band(int band_idx)
{
    int old_band_idx = receiver.band_idx;

	if (last_freq[band_idx] == 0)
		last_freq[band_idx] = BANDS[band_idx].from;
	
	last_freq[old_band_idx] = receiver.freq;

	receiver.band_idx = band_idx;
	receiver.freq = last_freq[band_idx];
	receiver.step = BANDS[band_idx].step;

	if (BANDS[band_idx].mode == FM_CURRENT_MODE)
		receiver_set_fm(BANDS[band_idx].from, BANDS[band_idx].to, receiver.freq,
						BANDS[band_idx].step);
	else
		receiver_set_am(BANDS[band_idx].from, BANDS[band_idx].to, receiver.freq,
						BANDS[band_idx].step);
}
