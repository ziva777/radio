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
	return (rgba.alpha != 0) &&
			!(rgba.red == 0 && rgba.green == 0 && rgba.blue == 0);
}

static void fatal(char *msg)
{
	fprintf(stderr, "error: %s\n", msg); 
	exit(1);
}

/*
 * Flip bits order in byte.
 */
static inline uint8_t reverse_bits(uint8_t b) {
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
	RGBA32		   *buf;
	uint8_t		   *data;
	size_t			n;
	int				i,
					j;

	buf = malloc(width * sizeof(RGBA32));	/* read line by line */
	data = malloc(width * height);
	j = 0;

	while ((n = fread(buf, sizeof(RGBA32), width, fp)) != 0)
	{
		if (n != width)
			fatal("invalid file dimention \"width\"");

		if (j == height)
			fatal("invalid file dimention \"height\"");

		for (i = 0; i < width; ++i)
			data[j*width + i] = is_visible(buf[i]);

		j++;
	}

	free(buf);

	return data;
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
	int				i,
					j,
					k;
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
		fprintf(fp, "\t");

		for (j = 0; j < height; j += 8)
		{
			b = 0;

			for (k = 0; k < 8; ++k)
				//b |= (data[(j + k) * width + i]) << k;
				if (data[(j + k) * width + i])
					b |= 1 << k;

			fprintf(fp, "0x%02X, ", reverse_bits(b));
		}

		fprintf(fp, "\t\n");
	}

	fprintf(fp, "};");

	fclose(fp);
	printf("File \"%s\" is created succesfully.\n", filename);
}

/*static void to_bitmat_mask(char *file, uint8_t *data, int width, int height)
{
	int				i,
					j;

	for (i = 0; i < width; ++i)
	{
		uint8_t		byte = 0;
		int			byte_idx = 7;

		for (j = 0; j < height; ++j)
		{
			if (data[j * width + i])
				byte |= (1 << byte_idx);

			if (byte_idx-- == 0)
			{
				printf("0x%02X, ", byte);
				byte_idx = 7;
				byte = 0;
			}
		}

		if (byte_idx != 7)
			printf("0x%02X, ", byte);

		printf("\n");
	}
}*/

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
