#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct RGBA32
{
	uint8_t		red;
	uint8_t		green;
	uint8_t		blue;
	uint8_t		alpha;
} RGBA32;

static inline int is_visible(RGBA32 rgba)
{
	return rgba.red != 0 || rgba.green != 0 || rgba.blue != 0;
	//return (rgba.alpha != 0) ||
	//		!(rgba.red == 0 && rgba.green == 0 && rgba.blue == 0);
}

static inline void fatal(char *msg)
{
	fprintf(stderr, "error: %s\n", msg); 
	exit(1);
}

/*
 * Flip bits order in byte.
 */
static inline uint8_t reverse_bits(uint8_t b)
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

/*
 * Convert input image file into byte mask.
 * Visible pixels are bytes not eq to 0.
 *
 * Return malloc'd buffer. It is up to caller to free it.
 *
 * NOTE:
 * - all pure black pixels consider to be invisible
 */
static uint8_t * byte_mask(FILE *fp, int width, int height)
{
	RGBA32		   buf;
	uint8_t		   *p1, *p2;
	size_t			n;
	size_t			n_bytes;

	p1 = p2 = malloc(width * height);
	n_bytes = 0;

	while ((n = fread(&buf, sizeof(buf), 1, fp)) != 0)
	{
		*p2++ = is_visible(buf);
		if (++n_bytes > (width * height))
			fatal("invalid size specifyed");
	}

	return p1;
}

static void normalize_filename(char *filename, size_t size)
{
	while (size-- != 0)
	{
		if ((filename[size]) == '.')
			filename[size] = '_';
		else if ((filename[size]) == '-')
			filename[size] = '_';
	}
}

/*
 * Write bitmap file.
 */
static void to_bitmat_mask(char *file, uint8_t *data, int width, int height)
{
	int				i, j, k;
	uint8_t			b;
	FILE		   *fp;
	char			filename[255];

	normalize_filename(file, strlen(file));
	snprintf(filename, sizeof(filename), "%s.c", file);
	fp = fopen(filename, "w");
	if (fp == NULL)
	{
		fprintf(stderr, "could not create file \"%s\" : ", filename);
		perror("error: ");
		exit(2);
	}

	fprintf(fp, "const uint8_t %s[] PROGMEM =\n{\n", file);

	for (i = 0; i < width; ++i)
	{
		for (j = 0; j < height; j)
		{
			b = 0;
			for (k = 0; k < 8; ++k)
			{
				if (j * width + i < width * height)
				{
					if (data[j * width + i])
						b |= 1 << k;
				}

				++j;
			}
			fprintf(fp, "0x%02X, ", reverse_bits(b));
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "};");

	fclose(fp);
	printf("File \"%s\" is created succesfully.\n", filename);
}

/*
 * Process file
 */
static void process(char *file, FILE *fp, int width, int height)
{
	uint8_t		   *data;

	data = byte_mask(fp, width, height);
	to_bitmat_mask(file, data, width, height);

	free(data);
}

int main(int argc, char **argv)
{
	int			width,
				height;
	FILE	   *fp;

	if (argc != 4)
		fatal("wrong number of arguments");

	width = atoi(argv[1]);
	if (width == 0)
		fatal("cannot parse arg width");

	height = atoi(argv[2]);
	if (height == 0)
		fatal("cannot parse arg height");

	fp = fopen(argv[3], "r");
	if (fp == NULL)
		fatal("cannot open input file");

	printf("Converting %s (%dx%d)\n", argv[3], width, height);
	process(argv[3], fp, width, height);
	fclose(fp);

	return 0;
}
