#include "vfd.h"
#include "wiring.h"

constexpr uint8_t STX		= 0x02;
constexpr uint8_t HDR		= 0x44;
constexpr uint8_t DAD		= 0x00;
constexpr uint8_t CMD_SEND	= 0x46;
constexpr uint8_t CMD_DIM	= 0x58;

constexpr int SIZE          = 2048; /* Futaba 64x256 px display */
static uint8_t fb[SIZE];            /* actual frame to by draw */

static void send(uint8_t b);
static void begin_send(uint16_t pos, uint16_t size);
static uint8_t rfill_byte(uint8_t b);

void
vfd_init()
{
    /* Setup RDY pin */
	cbi(RDY_PORT, RDY_PIN);
	cbi(RDY_DDR, RDY_PIN);

	/* Setup RW pin */
	sbi(WR_PORT, WR_PIN);
	sbi(WR_DDR, WR_PIN);

	/* Setup DATA pin */
	sbi(D0_DDR, D0_PIN);
	sbi(D1_DDR, D1_PIN);
	sbi(D2_DDR, D2_PIN);
	sbi(D3_DDR, D3_PIN);
	sbi(D4_DDR, D4_PIN);
	sbi(D5_DDR, D5_PIN);
	sbi(D6_DDR, D6_PIN);
	sbi(D7_DDR, D7_PIN);

    begin_send(0, SIZE);
	for (int i = 0; i < SIZE; i++)
		send(0x00);
}

void
vfd_sync()
{
    begin_send(0, SIZE);
    for (int i = 0; i < SIZE; i++)
        send(fb[i]);
}

void vfd_clb()
{
	memset(fb, 0, sizeof(fb));
}

void vfd_draw(Point p, Sprite sp, Blend b, bool invert)
{
	uint8_t			nbytes = sp.height / 8 + (sp.height % 8 != 0 ? 1 : 0);
	const uint8_t  *src = sp.data;
	uint64_t	   *dsp = reinterpret_cast<uint64_t *>(fb);
	uint8_t			frac = p.y % 8;
	uint8_t			trunc = p.y / 8;
	int16_t			idx = p.x;
	union {
		uint8_t		u8[8];
		uint64_t	u64;
	} cast;

	for (uint16_t i = 0; i < sp.width; i++) {
		cast.u64 = 0;
		memcpy_P(cast.u8, src, nbytes);
		src += nbytes;

        /* invert bits */
        if (invert) {
            for (int i = 0; i < nbytes; i++)
                cast.u8[i] = ~cast.u8[i];

            cast.u8[nbytes - 1] &= ~rfill_byte(8 - sp.height % 8);
        }

		/* move by x8 steps */
		if (trunc != 0)
			cast.u64 <<= trunc * 8;

		/* move by < 8 steps */
		if (frac != 0 && cast.u64 != 0) {
			for (uint8_t k = 7; k != 0; k--)
				cast.u8[k] = cast.u8[k] >> frac |
								(cast.u8[k - 1] & rfill_byte(p.y)) << (8 - frac);

			cast.u8[0] = cast.u8[0] >> frac;
		}

		idx = p.x + i;
		if (0 <= idx && idx <= 255) {
			switch (b) {
			case Blend::ADD:
				dsp[idx] |= cast.u64;
				break;
			case Blend::XOR:
				dsp[idx] ^= cast.u64;
				break;
			case Blend::OVER:
			default:
				dsp[idx] = cast.u64;
				break;
			}
		}
	}
}

static void
send(uint8_t b)
{
    while (!(RDY_PINS & (1 << RDY_PIN)));

    cbi(WR_PORT, WR_PIN);
    D0_PORT = b;
    sbi(WR_PORT, WR_PIN);
}

static void
begin_send(uint16_t pos, uint16_t size)
{
    send(STX);
    send(HDR);
    send(DAD);
    send(CMD_SEND);
    send(lowByte(pos));
    send(highByte(pos));
    send(lowByte(size));
    send(highByte(size));
}

static uint8_t
rfill_byte(uint8_t b)
{
    switch (b)
    {
    case 1:
        return B00000001;
    case 2:
        return B00000011;
    case 3:
        return B00000111;
    case 4:
        return B00001111;
    case 5:
        return B00011111;
    case 6:
        return B00111111;
    case 7:
        return B01111111;

    default:
        return B11111111;
    }
}