#ifndef INCLUDE_JRC_BC7_H
#define INCLUDE_JRC_BC7_H

void jrcDecodeBc7Block(unsigned char *rgbaOut, const unsigned char *blockIn);
void jrcEncodeBc7Block(unsigned char *blockOut, const unsigned char *rgbaIn);

#endif
#ifdef JRC_BC7_IMPLEMENTATION
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define BIT_MASK(x)  (0xFFFFFFFF >> (32 - (x)))
#define BIT_SHIFT(x) (1 << (x))

#define Vec2Add(r, a, b) ((r)[0] = (a)[0] + (b)[0], (r)[1] = (a)[1] + (b)[1])
#define Vec3Add(r, a, b) ((r)[0] = (a)[0] + (b)[0], (r)[1] = (a)[1] + (b)[1], (r)[2] = (a)[2] + (b)[2])
#define Vec4Add(r, a, b) ((r)[0] = (a)[0] + (b)[0], (r)[1] = (a)[1] + (b)[1], (r)[2] = (a)[2] + (b)[2], (r)[3] = (a)[3] + (b)[3]))

#define Vec2Clear(r) ((r)[0] = (r)[1] = 0)
#define Vec3Clear(r) ((r)[0] = (r)[1] = (r)[2] = 0)
#define Vec4Clear(r) ((r)[0] = (r)[1] = (r)[2] = (r)[3] = 0)

#define Vec3Copy(r, a) ((r)[0] = (a)[0], (r)[1] = (a)[1], (r)[2] = (a)[2])
#define Vec4Copy(r, a) ((r)[0] = (a)[0], (r)[1] = (a)[1], (r)[2] = (a)[2], (r)[3] = (a)[3])

#define Vec3Scale(r, s, v) ((r)[0] = (s) * (v)[0], (r)[1] = (s) * (v)[1], (r)[2] = (s) * (v)[2])

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#define ABS(x)   ((x) < 0 ? -(x) : (x))

#define NUM_MODES 8
#define NUM_ENDPOINTS 2
#define MAX_SUBSETS 3
#define MAX_INTERPS 16

// The following tables are all from http://www.opengl.org/registry/specs/ARB/texture_compression_bptc.txt

enum
{
	MODEINFO_MODE = 0,
	MODEINFO_SUBSETS_PER_PARTITION,
	MODEINFO_PARTITION_BITS,
	MODEINFO_ROTATION_BITS,
	MODEINFO_INDEX_SELECTION_BITS,
	MODEINFO_COLOR_BITS,
	MODEINFO_ALPHA_BITS,
	MODEINFO_ENDPOINT_P_BITS,
	MODEINFO_SHARED_P_BITS,
	MODEINFO_INDEX_BITS_PER_ELEMENT,
	MODEINFO_SECONDARY_INDEX_BITS_PER_ELEMENT,
	MODEINFO_NUM_MODEINFOS
};

static uint8_t modeInfo[NUM_MODES][MODEINFO_NUM_MODEINFOS] =
//  Mode NS PB RB ISB CB AB EPB SPB IB IB2
//  ---- -- -- -- --- -- -- --- --- -- ---
  {{0,   3, 4, 0, 0,  4, 0, 1,  0,  3, 0},
   {1,   2, 6, 0, 0,  6, 0, 0,  1,  3, 0},
   {2,   3, 6, 0, 0,  5, 0, 0,  0,  2, 0},
   {3,   2, 6, 0, 0,  7, 0, 1,  0,  2, 0},
   {4,   1, 0, 2, 1,  5, 6, 0,  0,  2, 3},
   {5,   1, 0, 2, 0,  7, 8, 0,  0,  2, 2},
   {6,   1, 0, 0, 0,  7, 7, 1,  0,  4, 0},
   {7,   2, 6, 0, 0,  5, 5, 1,  0,  2, 0}};

static uint8_t partitionSubsets[2][64][16] = 
	{{{0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},
	  {0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1},
	  {0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1},
	  {0,0,0,1,0,0,1,1,0,0,1,1,0,1,1,1},
	  {0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1},
	  {0,0,1,1,0,1,1,1,0,1,1,1,1,1,1,1},
	  {0,0,0,1,0,0,1,1,0,1,1,1,1,1,1,1},
	  {0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1},
	  {0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1},
	  {0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1},
	  {0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1},
	  {0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1},
	  {0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1},
	  {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
	  {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1},
	  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
	  {0,0,0,0,1,0,0,0,1,1,1,0,1,1,1,1},
	  {0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0},
	  {0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0},
	  {0,1,1,1,0,0,1,1,0,0,0,1,0,0,0,0},
	  {0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0},
	  {0,0,0,0,1,0,0,0,1,1,0,0,1,1,1,0},
	  {0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0},
	  {0,1,1,1,0,0,1,1,0,0,1,1,0,0,0,1},
	  {0,0,1,1,0,0,0,1,0,0,0,1,0,0,0,0},
	  {0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0},
	  {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
	  {0,0,1,1,0,1,1,0,0,1,1,0,1,1,0,0},
	  {0,0,0,1,0,1,1,1,1,1,1,0,1,0,0,0},
	  {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
	  {0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0},
	  {0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0},
	  {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
	  {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},
	  {0,1,0,1,1,0,1,0,0,1,0,1,1,0,1,0},
	  {0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0},
	  {0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0},
	  {0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,0},
	  {0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1},
	  {0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,1},
	  {0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0},
	  {0,0,0,1,0,0,1,1,1,1,0,0,1,0,0,0},
	  {0,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0},
	  {0,0,1,1,1,0,1,1,1,1,0,1,1,1,0,0},
	  {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0},
	  {0,0,1,1,1,1,0,0,1,1,0,0,0,0,1,1},
	  {0,1,1,0,0,1,1,0,1,0,0,1,1,0,0,1},
	  {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},
	  {0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,0},
	  {0,0,1,0,0,1,1,1,0,0,1,0,0,0,0,0},
	  {0,0,0,0,0,0,1,0,0,1,1,1,0,0,1,0},
	  {0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,0},
	  {0,1,1,0,1,1,0,0,1,0,0,1,0,0,1,1},
	  {0,0,1,1,0,1,1,0,1,1,0,0,1,0,0,1},
	  {0,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0},
	  {0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,0},
	  {0,1,1,0,1,1,0,0,1,1,0,0,1,0,0,1},
	  {0,1,1,0,0,0,1,1,0,0,1,1,1,0,0,1},
	  {0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1},
	  {0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,1},
	  {0,0,0,0,1,1,1,1,0,0,1,1,0,0,1,1},
	  {0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0},
	  {0,0,1,0,0,0,1,0,1,1,1,0,1,1,1,0},
	  {0,1,0,0,0,1,0,0,0,1,1,1,0,1,1,1}},
	 {{0,0,1,1,0,0,1,1,0,2,2,1,2,2,2,2},
	  {0,0,0,1,0,0,1,1,2,2,1,1,2,2,2,1},
	  {0,0,0,0,2,0,0,1,2,2,1,1,2,2,1,1},
	  {0,2,2,2,0,0,2,2,0,0,1,1,0,1,1,1},
	  {0,0,0,0,0,0,0,0,1,1,2,2,1,1,2,2},
	  {0,0,1,1,0,0,1,1,0,0,2,2,0,0,2,2},
	  {0,0,2,2,0,0,2,2,1,1,1,1,1,1,1,1},
	  {0,0,1,1,0,0,1,1,2,2,1,1,2,2,1,1},
	  {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2},
	  {0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2},
	  {0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2},
	  {0,0,1,2,0,0,1,2,0,0,1,2,0,0,1,2},
	  {0,1,1,2,0,1,1,2,0,1,1,2,0,1,1,2},
	  {0,1,2,2,0,1,2,2,0,1,2,2,0,1,2,2},
	  {0,0,1,1,0,1,1,2,1,1,2,2,1,2,2,2},
	  {0,0,1,1,2,0,0,1,2,2,0,0,2,2,2,0},
	  {0,0,0,1,0,0,1,1,0,1,1,2,1,1,2,2},
	  {0,1,1,1,0,0,1,1,2,0,0,1,2,2,0,0},
	  {0,0,0,0,1,1,2,2,1,1,2,2,1,1,2,2},
	  {0,0,2,2,0,0,2,2,0,0,2,2,1,1,1,1},
	  {0,1,1,1,0,1,1,1,0,2,2,2,0,2,2,2},
	  {0,0,0,1,0,0,0,1,2,2,2,1,2,2,2,1},
	  {0,0,0,0,0,0,1,1,0,1,2,2,0,1,2,2},
	  {0,0,0,0,1,1,0,0,2,2,1,0,2,2,1,0},
	  {0,1,2,2,0,1,2,2,0,0,1,1,0,0,0,0},
	  {0,0,1,2,0,0,1,2,1,1,2,2,2,2,2,2},
	  {0,1,1,0,1,2,2,1,1,2,2,1,0,1,1,0},
	  {0,0,0,0,0,1,1,0,1,2,2,1,1,2,2,1},
	  {0,0,2,2,1,1,0,2,1,1,0,2,0,0,2,2},
	  {0,1,1,0,0,1,1,0,2,0,0,2,2,2,2,2},
	  {0,0,1,1,0,1,2,2,0,1,2,2,0,0,1,1},
	  {0,0,0,0,2,0,0,0,2,2,1,1,2,2,2,1},
	  {0,0,0,0,0,0,0,2,1,1,2,2,1,2,2,2},
	  {0,2,2,2,0,0,2,2,0,0,1,2,0,0,1,1},
	  {0,0,1,1,0,0,1,2,0,0,2,2,0,2,2,2},
	  {0,1,2,0,0,1,2,0,0,1,2,0,0,1,2,0},
	  {0,0,0,0,1,1,1,1,2,2,2,2,0,0,0,0},
	  {0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0},
	  {0,1,2,0,2,0,1,2,1,2,0,1,0,1,2,0},
	  {0,0,1,1,2,2,0,0,1,1,2,2,0,0,1,1},
	  {0,0,1,1,1,1,2,2,2,2,0,0,0,0,1,1},
	  {0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2},
	  {0,0,0,0,0,0,0,0,2,1,2,1,2,1,2,1},
	  {0,0,2,2,1,1,2,2,0,0,2,2,1,1,2,2},
	  {0,0,2,2,0,0,1,1,0,0,2,2,0,0,1,1},
	  {0,2,2,0,1,2,2,1,0,2,2,0,1,2,2,1},
	  {0,1,0,1,2,2,2,2,2,2,2,2,0,1,0,1},
	  {0,0,0,0,2,1,2,1,2,1,2,1,2,1,2,1},
	  {0,1,0,1,0,1,0,1,0,1,0,1,2,2,2,2},
	  {0,2,2,2,0,1,1,1,0,2,2,2,0,1,1,1},
	  {0,0,0,2,1,1,1,2,0,0,0,2,1,1,1,2},
	  {0,0,0,0,2,1,1,2,2,1,1,2,2,1,1,2},
	  {0,2,2,2,0,1,1,1,0,1,1,1,0,2,2,2},
	  {0,0,0,2,1,1,1,2,1,1,1,2,0,0,0,2},
	  {0,1,1,0,0,1,1,0,0,1,1,0,2,2,2,2},
	  {0,0,0,0,0,0,0,0,2,1,1,2,2,1,1,2},
	  {0,1,1,0,0,1,1,0,2,2,2,2,2,2,2,2},
	  {0,0,2,2,0,0,1,1,0,0,1,1,0,0,2,2},
	  {0,0,2,2,1,1,2,2,1,1,2,2,0,0,2,2},
	  {0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,2},
	  {0,0,0,2,0,0,0,1,0,0,0,2,0,0,0,1},
	  {0,2,2,2,1,2,2,2,0,2,2,2,1,2,2,2},
	  {0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2},
	  {0,1,1,1,2,0,1,1,2,2,0,1,2,2,2,0}}};

static uint8_t anchors[3][64] = 
	{{15,15,15,15,15,15,15,15,
	  15,15,15,15,15,15,15,15,
	  15, 2, 8, 2, 2, 8, 8,15,
	   2, 8, 2, 2, 8, 8, 2, 2,
	  15,15, 6, 8, 2, 8,15,15,
	   2, 8, 2, 2, 2,15,15, 6,
	   6, 2, 6, 8,15,15, 2, 2,
	  15,15,15,15,15, 2, 2,15},
	 { 3, 3,15,15, 8, 3,15,15,
	   8, 8, 6, 6, 6, 5, 3, 3,
	   3, 3, 8,15, 3, 3, 6,10,
	   5, 8, 8, 6, 8, 5,15,15,
	   8,15, 3, 5, 6,10, 8,15,
	  15, 3,15, 5,15,15,15,15,
	   3,15, 5, 5, 5, 8, 5,10,
	   5,10, 8,13,15,12, 3, 3},
	 {15, 8, 8, 3,15,15, 3, 8,
	  15,15,15,15,15,15,15, 8,
	  15, 8,15, 3,15, 8,15, 8,
 	   3,15, 6,10,15,15,10, 8,
	  15, 3,15,10,10, 8, 9,10,
	   6,15, 8,15, 3, 6, 6, 8,
	  15, 3,15,15,15,15,15,15,
	  15,15,15,15, 3,15,15, 8}};

static uint8_t interpFactors[3][16] =
	{{0, 21, 43, 64,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	 {0,  9, 18, 27, 37, 46, 55, 64,  0,  0,  0,  0,  0,  0,  0,  0},
	 {0,  4,  9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64}};


static uint32_t GetBits(const uint8_t *in, int *offset, int numBits)
{
	int offMod, result, currPos;

	if (numBits == 0)
		return 0;

	in     += *offset / 8;
	offMod  = *offset % 8;

	if (offMod)
	{
		result = *in++ >> offMod;
		currPos = 8 - offMod;
	}
	else
	{
		result = 0;
		currPos = 0;
	}

	while (currPos < numBits)
	{
		result |= *in++ << currPos;
		currPos += 8;
	}

	result &= BIT_MASK(numBits);
	*offset += numBits;

	return result;
}

static void SetBits(uint8_t *out, int *offset, uint32_t value, int numBits)
{
	int offMod;

	if (numBits == 0)
		return;

	out    += *offset / 8;
	offMod  = *offset % 8;

	if (offMod)
	{
		*out++ |= (((value & BIT_MASK(numBits)) << offMod) & 0xff);
		
		if (8 - offMod > numBits)
		{
			*offset += numBits;
			return;
		}
		else
		{
			value >>= 8 - offMod;
			numBits -= 8 - offMod;
			*offset += 8 - offMod;
		}
	}
	
	while(numBits >= 8)
	{
		*out++ = (value & 0xff);
		value >>= 8;
		numBits -= 8;
		*offset += 8;
		
	}
	
	if (numBits > 0)
	{
		*out++ = (value & BIT_MASK(numBits));
		*offset += numBits;
	}
}

static void DecodeColors(uint8_t color[MAX_SUBSETS][NUM_ENDPOINTS][4], const uint8_t *blockIn, int *bitPos, int numSubsets, int pBitMode, int colorBits, int alphaBits)
{
	int component, subset, endpoint;

	// decode endpoints
	for (component = 0; component < 3; component++)
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				color[subset][endpoint][component] = GetBits(blockIn, bitPos, colorBits) << (8 - colorBits);

	if (alphaBits)
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				color[subset][endpoint][3] = GetBits(blockIn, bitPos, alphaBits) << (8 - alphaBits);
	else
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				color[subset][endpoint][3] = 255;

	if (pBitMode == 1)
	{
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
			{
				uint8_t b = GetBits(blockIn, bitPos, 1);
				uint8_t cpb = b << (7 - colorBits);
				uint8_t apb = b << (7 - alphaBits);

				color[subset][endpoint][0] |= cpb;
				color[subset][endpoint][1] |= cpb;
				color[subset][endpoint][2] |= cpb;
				color[subset][endpoint][3] |= apb;
			}
	}
	else if (pBitMode == 2)
	{
		for (subset = 0; subset < numSubsets; subset++)
		{
			uint8_t b = GetBits(blockIn, bitPos, 1);
			uint8_t cpb = b << (7 - colorBits);
			uint8_t apb = b << (7 - alphaBits);

			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
			{
				color[subset][endpoint][0] |= cpb;
				color[subset][endpoint][1] |= cpb;
				color[subset][endpoint][2] |= cpb;
				color[subset][endpoint][3] |= apb;
			}
		}
	}

	// replicate MSBs in LSBs
	for (subset = 0; subset < numSubsets; subset++)
		for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
		{
			color[subset][endpoint][0] |= color[subset][endpoint][0] >> (colorBits + (pBitMode ? 1 : 0));
			color[subset][endpoint][1] |= color[subset][endpoint][1] >> (colorBits + (pBitMode ? 1 : 0));
			color[subset][endpoint][2] |= color[subset][endpoint][2] >> (colorBits + (pBitMode ? 1 : 0));
			color[subset][endpoint][3] |= color[subset][endpoint][3] >> (alphaBits + (pBitMode ? 1 : 0));
		}
}

static void Interp64Rgb(uint8_t *r, uint8_t *x, uint8_t *y, int a)
{
	int b = 64 - a;
	
	r[0] = (x[0] * b + y[0] * a + 32) >> 6;
	r[1] = (x[1] * b + y[1] * a + 32) >> 6;
	r[2] = (x[2] * b + y[2] * a + 32) >> 6;
}

static void Interp64Alpha(uint8_t *r, uint8_t *x, uint8_t *y, int a)
{
	int b = 64 - a;

	r[3] = (x[3] * b + y[3] * a + 32) >> 6;
}

static void InterpColors(uint8_t interpColor[MAX_SUBSETS][MAX_INTERPS][4], uint8_t color[MAX_SUBSETS][NUM_ENDPOINTS][4], int numSubsets, int indexSelection, int indexBits, int index2Bits)
{
	int rgbIndexBits, alphaIndexBits, subset, i;

	rgbIndexBits = indexSelection ? index2Bits : indexBits;
	alphaIndexBits = indexSelection ? indexBits : (index2Bits ? index2Bits : indexBits);

	for (subset = 0; subset < numSubsets; subset++)
		for (i = 0; i < BIT_SHIFT(rgbIndexBits); i++)
			Interp64Rgb(interpColor[subset][i], color[subset][0], color[subset][1], interpFactors[rgbIndexBits - 2][i]);

	for (subset = 0; subset < numSubsets; subset++)
		for (i = 0; i < BIT_SHIFT(alphaIndexBits); i++)
			Interp64Alpha(interpColor[subset][i], color[subset][0], color[subset][1], interpFactors[alphaIndexBits - 2][i]);
}

static void CopyRGB(uint8_t out[4], const uint8_t in[4])
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

static void CopyAlpha(uint8_t out[4], const uint8_t in[4])
{
	out[3] = in[3];
}

static void CopyRGBA(uint8_t out[4], const uint8_t in[4])
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	out[3] = in[3];
}

static void DecodeIndexes(uint8_t *rgbaOut, void (*copyFunc)(uint8_t out[4], const uint8_t in[4]), const uint8_t *blockIn, int *bitPos, uint8_t interpColor[MAX_SUBSETS][MAX_INTERPS][4], int numSubsets, int partition, int indexBits)
{
	int i, subset, anchorIndex, colorIndex;
	uint8_t *subsetTable = (numSubsets > 1) ? partitionSubsets[numSubsets - 2][partition] : NULL;

	for (i = 0; i < 16; i++)
	{
		subset = subsetTable ? subsetTable[i] : 0;
		anchorIndex = subset ? anchors[subset - ((numSubsets > 2) ? 0 : 1)][partition] : 0;

		colorIndex = GetBits(blockIn, bitPos, indexBits - ((i == anchorIndex) ? 1 : 0));

		copyFunc(rgbaOut, interpColor[subset][colorIndex]);
		rgbaOut += 4;
	}
}

void jrcDecodeBc7Block(uint8_t *rgbaOut, const uint8_t *blockIn)
{
	uint8_t color[MAX_SUBSETS][NUM_ENDPOINTS][4];
	uint8_t interpColor[MAX_SUBSETS][MAX_INTERPS][4];
	int numSubsets, partition, rotation, indexSelection, colorBits;
	int alphaBits, pBitMode, indexBits, index2Bits;
	int mode, i;
	int bitPos;

	for (mode = 0; mode < NUM_MODES; mode++)
		if ((blockIn[0] & BIT_MASK(mode + 1)) == BIT_SHIFT(mode))
			break;

	if (mode == NUM_MODES)
	{
		// illegal mode, return zeroes
		for (i = 0; i < 64; i++)
			rgbaOut[i] = 0;
		return;
	}

	bitPos = mode + 1;

	numSubsets = modeInfo[mode][MODEINFO_SUBSETS_PER_PARTITION];
	partition = GetBits(blockIn, &bitPos, modeInfo[mode][MODEINFO_PARTITION_BITS]);
	rotation = GetBits(blockIn, &bitPos, modeInfo[mode][MODEINFO_ROTATION_BITS]);
	indexSelection = GetBits(blockIn, &bitPos, modeInfo[mode][MODEINFO_INDEX_SELECTION_BITS]);
	colorBits = modeInfo[mode][MODEINFO_COLOR_BITS];
	alphaBits = modeInfo[mode][MODEINFO_ALPHA_BITS];
	pBitMode = modeInfo[mode][MODEINFO_ENDPOINT_P_BITS] + (modeInfo[mode][MODEINFO_SHARED_P_BITS] << 1);
	indexBits = modeInfo[mode][MODEINFO_INDEX_BITS_PER_ELEMENT];
	index2Bits = modeInfo[mode][MODEINFO_SECONDARY_INDEX_BITS_PER_ELEMENT];

	DecodeColors(color, blockIn, &bitPos, numSubsets, pBitMode, colorBits, alphaBits);
	InterpColors(interpColor, color, numSubsets, indexSelection, indexBits, index2Bits);

	if (indexSelection)
	{
		DecodeIndexes(rgbaOut, CopyAlpha, blockIn, &bitPos, interpColor, numSubsets, partition, indexBits);
		DecodeIndexes(rgbaOut, CopyRGB,   blockIn, &bitPos, interpColor, numSubsets, partition, index2Bits);
	}
	else if (index2Bits)
	{
		DecodeIndexes(rgbaOut, CopyRGB,   blockIn, &bitPos, interpColor, numSubsets, partition, indexBits);
		DecodeIndexes(rgbaOut, CopyAlpha, blockIn, &bitPos, interpColor, numSubsets, partition, index2Bits);
	}
	else
		DecodeIndexes(rgbaOut, CopyRGBA, blockIn, &bitPos, interpColor, numSubsets, partition, indexBits);

	if (rotation)
		for (i = 0; i < 16; i++)
		{
			uint8_t newAlpha        = rgbaOut[i*4+rotation-1];
			rgbaOut[i*4+rotation-1] = rgbaOut[i*4+3];
			rgbaOut[i*4+3]          = newAlpha;
		}
}

// ITU.BT-709 HDTV studio production in Y'CbCr for non-linear signals
// according to http://www.martinreddy.net/gfx/faqs/colorconv.faq
//
// Altered slightly to fit (0..1) and (-0.5..0.5)
static void Rgb8ToYcbcr32f(float ycbcr32f[3], const uint8_t rgb8[3])
{
	float rgb32f[3];

	rgb32f[0] = rgb8[0] / 255.0f;
	rgb32f[1] = rgb8[1] / 255.0f;
	rgb32f[2] = rgb8[2] / 255.0f;

	ycbcr32f[0] =  0.2125f * rgb32f[0] + 0.7154f * rgb32f[1] + 0.0721f * rgb32f[2];
	ycbcr32f[1] = -0.1145f * rgb32f[0] - 0.3855f * rgb32f[1] + 0.5000f * rgb32f[2];
	ycbcr32f[2] =  0.5000f * rgb32f[0] - 0.4542f * rgb32f[1] - 0.0458f * rgb32f[2];
}

static void Ycbcr32fToRgb8(uint8_t rgb8[3], const float ycbcr32f[3])
{
	float rgb32f[3];

	rgb32f[0] = ycbcr32f[0]                         + 1.5750f * ycbcr32f[2];
	rgb32f[1] = ycbcr32f[0] - 0.1870f * ycbcr32f[1] - 0.4678f * ycbcr32f[2];
	rgb32f[2] = ycbcr32f[0] + 1.8558f * ycbcr32f[1];

	// Ycbcr allows for values outside (0..1) once converted to RGB, so clamp.
	rgb8[0] = CLAMP(rgb32f[0], 0.0f, 1.0f) * 255;
	rgb8[1] = CLAMP(rgb32f[1], 0.0f, 1.0f) * 255;
	rgb8[2] = CLAMP(rgb32f[2], 0.0f, 1.0f) * 255;
	
}

static float Rgb8ToY(const uint8_t rgb8[3])
{
	float rgb32f[3];

	rgb32f[0] = rgb8[0] / 255.0f;
	rgb32f[1] = rgb8[1] / 255.0f;
	rgb32f[2] = rgb8[2] / 255.0f;

	return 0.2125f * rgb32f[0] + 0.7154f * rgb32f[1] + 0.0721f * rgb32f[2];
}

static float DistanceBetweenYcbcr(float ycbcr1[3], float ycbcr2[3])
{
	float d[3];

	d[0] = ycbcr1[0] - ycbcr2[0];
	d[1] = ycbcr1[1] - ycbcr2[1];
	d[2] = ycbcr1[2] - ycbcr2[2];

	return d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
}

static float DistanceBetweenRgb8(const uint8_t *color1, const uint8_t *color2)
{
	float ycbcr1[3], ycbcr2[3];

	Rgb8ToYcbcr32f(ycbcr1, color1);
	Rgb8ToYcbcr32f(ycbcr2, color2);

	return DistanceBetweenYcbcr(ycbcr1, ycbcr2);
}

static float DistanceBetweenRgba8(const uint8_t *color1, const uint8_t *color2)
{
	float ycbcr1[3], ycbcr2[3], alphaDiff;

	Rgb8ToYcbcr32f(ycbcr1, color1);
	Rgb8ToYcbcr32f(ycbcr2, color2);
	alphaDiff = (color1[3] - color2[3]) / 255.0f;
	alphaDiff *= alphaDiff;

	return DistanceBetweenYcbcr(ycbcr1, ycbcr2) + alphaDiff;
}

static float DistanceBetweenAlpha(const uint8_t *color1, const uint8_t *color2)
{
	return ABS(color1[3] - color2[3]);
}

static void FindEndpointsRgb(uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], const uint8_t *rgbaIn, int numSubsets, int partition)
{
	uint8_t *subsetTable = (numSubsets > 1) ? partitionSubsets[numSubsets - 2][partition] : NULL;
	int subset;

	for (subset = 0; subset < numSubsets; subset++)
	{
		float avgY, minY, maxY, ycbcr[2][3];
		int count, count2;
		int i;

		// find min, max, and average Y
		minY = 1.0f;
		maxY = 0.0f;
		avgY = 0.0f;
		count = 0;
		for (i = 0; i < 16; i++)
		{
			float y;

			if (subsetTable && subsetTable[i] != subset)
				continue;

			// ignore transparent pixels
			if (rgbaIn[i*4+3] == 0)
				continue;

			y = Rgb8ToY(&rgbaIn[i*4]);
			y = pow(y, 2.2);
			minY = MIN(minY, y);
			maxY = MAX(maxY, y);
			avgY += y;
			count++;
		}
		avgY /= (float)(MAX(count, 1));

		// find average ycbcr below and above avgY
		Vec3Clear(ycbcr[0]);
		Vec3Clear(ycbcr[1]);
		count = count2 = 0;
		for (i = 0; i < 16; i++)
		{
			float currYcbcr[3];

			if (subsetTable && subsetTable[i] != subset)
				continue;

			// ignore transparent pixels
			if (rgbaIn[i*4+3] == 0)
				continue;

			Rgb8ToYcbcr32f(currYcbcr, &rgbaIn[i*4]);
			currYcbcr[0] = pow(currYcbcr[0], 2.2);

			if (currYcbcr[0] <= avgY)
			{
				Vec3Add(ycbcr[0], ycbcr[0], currYcbcr);
				count++;
			}

			if (currYcbcr[0] >= avgY)
			{
				Vec3Add(ycbcr[1], ycbcr[1], currYcbcr);
				count2++;
			}
		}

		Vec3Scale(ycbcr[0], 1.0f / MAX(count,  1), ycbcr[0]);
		Vec3Scale(ycbcr[1], 1.0f / MAX(count2, 1), ycbcr[1]);

		// nudge Ys away from center
		ycbcr[0][0] = minY * 0.5f + ycbcr[0][0] * 0.5f;
		ycbcr[1][0] = maxY * 0.5f + ycbcr[1][0] * 0.5f;

		ycbcr[0][0] = pow(ycbcr[0][0], 1.0/2.2);
		ycbcr[1][0] = pow(ycbcr[1][0], 1.0/2.2);

		// convert ycbcr to endpoint colors
		Ycbcr32fToRgb8(colors[subset][0], ycbcr[0]);
		Ycbcr32fToRgb8(colors[subset][1], ycbcr[1]);
	}
}

static void FindEndpointsAlpha(uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], const uint8_t *rgbaIn, int numSubsets, int partition)
{
	uint8_t *subsetTable = (numSubsets > 1) ? partitionSubsets[numSubsets - 2][partition] : NULL;
	int subset, i;

	for (subset = 0; subset < numSubsets; subset++)
	{
		colors[subset][0][3] = 255;
		colors[subset][1][3] = 0;

		for (i = 0; i < 16; i++)
		{
			if (subsetTable && subsetTable[i] != subset)
				continue;

			if (rgbaIn[i*4+3] < colors[subset][0][3]) colors[subset][0][3] = rgbaIn[i*4+3];
			if (rgbaIn[i*4+3] > colors[subset][1][3]) colors[subset][1][3] = rgbaIn[i*4+3];
		}
	}
}

static void EncodeIndexes(int indexes[16], int indexBits, const uint8_t *rgbaIn, uint8_t interpColors[MAX_SUBSETS][MAX_INTERPS][4], float (*compFunc)(const uint8_t *a, const uint8_t *b), int numSubsets, int partition)
{
	uint8_t *subsetTable = (numSubsets > 1) ? partitionSubsets[numSubsets - 2][partition] : NULL;
	int i, j;
	
	if (!indexBits)
		return;

	for (i = 0; i < 16; i++)
	{
		int nearestColor = 0;
		float lowestDiff;

		int subset = subsetTable ? subsetTable[i] : 0;

		lowestDiff = compFunc(interpColors[subset][0], &rgbaIn[i*4]);
		for (j = 1; j < BIT_SHIFT(indexBits); j++)
		{
			float diff = compFunc(interpColors[subset][j], &rgbaIn[i*4]);

			if (diff < lowestDiff)
			{
				lowestDiff = diff;
				nearestColor = j;
			}
		}

		indexes[i] = nearestColor;
	}
}

static void CorrectEndpoints(uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], uint8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int numSubsets, int colorBits, int alphaBits, int pBitsMode)
{
	int subset, endpoint, component, maxComponent;
	int cBits[4] = {colorBits, colorBits, colorBits, alphaBits};
	
	maxComponent = alphaBits ? 4 : 3;

	if (pBitsMode == 1)
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
			{
				pBits[subset][endpoint] = 0;
				for (component = 0; component < maxComponent; component++)
					pBits[subset][endpoint] += (colors[subset][endpoint][component] >> (8 - cBits[component])) & 1;

				pBits[subset][endpoint] = (pBits[subset][endpoint] >> 2) ? 1 : 0;
			}
	else if (pBitsMode == 2)
		for (subset = 0; subset < numSubsets; subset++)
		{
			pBits[subset][0] = 0;
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				for (component = 0; component < maxComponent; component++)
					pBits[subset][0] += (colors[subset][0][component] >> (8 - cBits[component])) & 1;

			pBits[subset][0] = (pBits[subset][0] / 3) ? 1 : 0; 
			pBits[subset][1] = pBits[subset][0];
		}

	for (subset = 0; subset < numSubsets; subset++)
		for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
			for (component = 0; component < maxComponent; component++)
			{
				colors[subset][endpoint][component] &= (BIT_MASK(cBits[component]) << (8 - cBits[component]));
				colors[subset][endpoint][component] |= colors[subset][endpoint][component] >> (cBits[component] + (pBitsMode ? 1 : 0));
			}

	if (pBitsMode)
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				for (component = 0; component < maxComponent; component++)
					colors[subset][endpoint][component] |= pBits[subset][endpoint] << (7 - cBits[component]);
}

static void InterpEndpoints(uint8_t interpColors[MAX_SUBSETS][MAX_INTERPS][4], uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], int numSubsets, int alphaBits, int indexBits, int index2Bits, int indexSelection)
{
	int subset, i;
	int colorIndexBits, alphaIndexBits;

	colorIndexBits = alphaIndexBits = indexBits;
	if (index2Bits)
	{
		if (indexSelection)
			colorIndexBits = index2Bits;
		else
			alphaIndexBits = index2Bits;
	}

	for (subset = 0; subset < numSubsets; subset++)
	{
		for (i = 0; i < BIT_SHIFT(colorIndexBits); i++)
			Interp64Rgb(interpColors[subset][i], colors[subset][0], colors[subset][1], interpFactors[colorIndexBits - 2][i]);

		for (i = 0; i < BIT_SHIFT(alphaIndexBits); i++)
			Interp64Alpha(interpColors[subset][i], colors[subset][0], colors[subset][1], interpFactors[alphaIndexBits - 2][i]);
	}
}

static void SwapRgb(uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], uint8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int subset)
{
	uint8_t swap;

	swap = colors[subset][0][0]; colors[subset][0][0] = colors[subset][1][0]; colors[subset][1][0] = swap;
	swap = colors[subset][0][1]; colors[subset][0][1] = colors[subset][1][1]; colors[subset][1][1] = swap;
	swap = colors[subset][0][2]; colors[subset][0][2] = colors[subset][1][2]; colors[subset][1][2] = swap;
	swap = pBits[subset][0];     pBits[subset][0] = pBits[subset][1];         pBits[subset][1] = swap;
}

static void SwapRgba(uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], uint8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int subset)
{
	uint8_t swap;

	swap = colors[subset][0][0]; colors[subset][0][0] = colors[subset][1][0]; colors[subset][1][0] = swap;
	swap = colors[subset][0][1]; colors[subset][0][1] = colors[subset][1][1]; colors[subset][1][1] = swap;
	swap = colors[subset][0][2]; colors[subset][0][2] = colors[subset][1][2]; colors[subset][1][2] = swap;
	swap = colors[subset][0][3]; colors[subset][0][3] = colors[subset][1][3]; colors[subset][1][3] = swap;
	swap = pBits[subset][0];     pBits[subset][0] = pBits[subset][1];         pBits[subset][1] = swap;
}

static void SwapAlpha(uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], uint8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int subset)
{
	uint8_t swap;

	swap = colors[subset][0][3]; colors[subset][0][3] = colors[subset][1][3]; colors[subset][1][3] = swap;
}

static void FixAnchorIndexes(uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], uint8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int indexes[16], int numSubsets, int partition, int indexBits, void (*swapFunc)(uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], uint8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int subset))
{
	uint8_t *subsetTable = (numSubsets > 1) ? partitionSubsets[numSubsets - 2][partition] : NULL;
	int subset, i;

	if (!indexBits)
		return;

	for (subset = 0; subset < numSubsets; subset++)
	{
		int anchorIndex = subset ? anchors[subset - ((numSubsets > 2) ? 0 : 1)][partition] : 0;

		if (indexes[anchorIndex] & BIT_SHIFT(indexBits - 1))
		{
			for (i = 0; i < 16; i++)
			{
				if ((subsetTable ? subsetTable[i] : 0) != subset)
					continue;

				indexes[i] = BIT_MASK(indexBits) - indexes[i];
			}

			swapFunc(colors, pBits, subset);
		}
	}
}

static void WriteRgba(uint8_t *blockOut, int *bitPos, uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], int numSubsets, int colorBits, int alphaBits)
{
	int component, subset, endpoint;
	int cBits[4] = {colorBits, colorBits, colorBits, alphaBits};

	for (component = 0; component < 4; component++)
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				SetBits(blockOut, bitPos, colors[subset][endpoint][component] >> (8 - cBits[component]), cBits[component]);
}

static void WritePBits(uint8_t *blockOut, int *bitPos, uint8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int numSubsets, int pBitMode)
{
	int subset, endpoint;

	if (pBitMode == 1)
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				SetBits(blockOut, bitPos, pBits[subset][endpoint], 1);
	else if (pBitMode == 2)
		for (subset = 0; subset < numSubsets; subset++)
			SetBits(blockOut, bitPos, pBits[subset][0], 1);
}

static void WriteIndexes(uint8_t *blockOut, int *bitPos, int indexes[16], int numSubsets, int partition, int indexBits)
{
	uint8_t *subsetTable = (numSubsets > 1) ? partitionSubsets[numSubsets - 2][partition] : NULL;
	int i;

	if (!indexBits)
		return;

	for (i = 0; i < 16; i++)
	{
		int subset = subsetTable ? subsetTable[i] : 0;
		int anchorIndex = subset ? anchors[subset - ((numSubsets > 2) ? 0 : 1)][partition] : 0;

		SetBits(blockOut, bitPos, indexes[i], indexBits - ((i == anchorIndex) ? 1 : 0));
	}
}

// pick a partition based on the largest distance between average ycbcr of subsets
int PickPartition(const uint8_t *rgbaIn, int numSubsets, int partitionBits, float *distanceOut)
{
	int partition, numPartitions, bestPartition, i;
	float bestDistance, blockYcbcrs[16][3];

	if (numSubsets < 2)
		return 0;

	numPartitions = BIT_SHIFT(partitionBits);

	for (i = 0; i < 16; i++)
		Rgb8ToYcbcr32f(blockYcbcrs[i], &rgbaIn[i*4]);

	bestPartition = 0;
	bestDistance = 0.0f;

	for (partition = 0; partition < numPartitions; partition++)
	{
		uint8_t *subsetTable = partitionSubsets[numSubsets - 2][partition];
		float distance, avgYcbcrs[MAX_SUBSETS][3];
		int counts[MAX_SUBSETS];
		int subset;

		for (subset = 0; subset < numSubsets; subset++)
		{
			counts[subset] = 0;
			Vec3Clear(avgYcbcrs[subset]);
		}

		for (i = 0; i < 16; i++)
		{
			subset = subsetTable[i];
			Vec3Add(avgYcbcrs[subset], avgYcbcrs[subset], blockYcbcrs[i]);
			counts[subset]++;
		}

		for (subset = 0; subset < numSubsets; subset++)
			Vec3Scale(avgYcbcrs[subset], 1.0f / MAX(counts[subset], 1), avgYcbcrs[subset]);

		distance = DistanceBetweenYcbcr(avgYcbcrs[0], avgYcbcrs[1]);
		if (numSubsets == 3)
			distance += DistanceBetweenYcbcr(avgYcbcrs[1], avgYcbcrs[2]) + DistanceBetweenYcbcr(avgYcbcrs[2], avgYcbcrs[0]);

		if (distance > bestDistance)
		{
			bestPartition = partition;
			bestDistance = distance;
		}
	}
	
	if (distanceOut)
		*distanceOut = bestDistance * (numSubsets == 3 ? 0.35f : 1.0f);

	return bestPartition;
}

static void EncodeBptcBlockMode(uint8_t *blockOut, const uint8_t *rgbaIn, int mode)
{
	uint8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4];
	uint8_t interpColors[MAX_SUBSETS][MAX_INTERPS][4];
	uint8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS];
	int indexes[16], indexes2[16];

	int numSubsets, partitionBits, rotationBits, indexSelectionBits, colorBits;
	int alphaBits, pBitMode, indexBits, index2Bits;

	int partition, rotation, indexSelection;

	int bitPos;

	numSubsets = modeInfo[mode][MODEINFO_SUBSETS_PER_PARTITION];
	partitionBits = modeInfo[mode][MODEINFO_PARTITION_BITS];
	rotationBits = modeInfo[mode][MODEINFO_ROTATION_BITS];
	indexSelectionBits = modeInfo[mode][MODEINFO_INDEX_SELECTION_BITS];
	colorBits = modeInfo[mode][MODEINFO_COLOR_BITS];
	alphaBits = modeInfo[mode][MODEINFO_ALPHA_BITS];
	pBitMode = modeInfo[mode][MODEINFO_ENDPOINT_P_BITS] + (modeInfo[mode][MODEINFO_SHARED_P_BITS] << 1);
	indexBits = modeInfo[mode][MODEINFO_INDEX_BITS_PER_ELEMENT];
	index2Bits = modeInfo[mode][MODEINFO_SECONDARY_INDEX_BITS_PER_ELEMENT];

	partition = PickPartition(rgbaIn, numSubsets, partitionBits, NULL);
	rotation = 0; // FIXME: use rotationBits
	indexSelection = (mode == 4) ? 1 : 0; // FIXME: use indexSelectionBits

	FindEndpointsRgb(colors, rgbaIn, numSubsets, partition);
	FindEndpointsAlpha(colors, rgbaIn, numSubsets, partition);
	CorrectEndpoints(colors, pBits, numSubsets, colorBits, alphaBits, pBitMode);
	InterpEndpoints(interpColors, colors, numSubsets, alphaBits, indexBits, index2Bits, indexSelection);

	EncodeIndexes(indexes,  indexBits,  rgbaIn, interpColors, mode > 5 ? DistanceBetweenRgba8 : indexSelection ? DistanceBetweenAlpha : DistanceBetweenRgb8,  numSubsets, partition);
	EncodeIndexes(indexes2, index2Bits, rgbaIn, interpColors, indexSelection ? DistanceBetweenRgb8  : DistanceBetweenAlpha, numSubsets, partition);
	FixAnchorIndexes(colors, pBits, indexes,  numSubsets, partition, indexBits,  mode > 5 ? SwapRgba : indexSelection ? SwapAlpha : SwapRgb);
	FixAnchorIndexes(colors, pBits, indexes2, numSubsets, partition, index2Bits, indexSelection ? SwapRgb :   SwapAlpha);

	bitPos = 0;
	SetBits(blockOut, &bitPos, BIT_SHIFT(mode), mode + 1);
	SetBits(blockOut, &bitPos, partition, partitionBits);
	SetBits(blockOut, &bitPos, rotation, rotationBits);
	SetBits(blockOut, &bitPos, indexSelection, indexSelectionBits);

	WriteRgba(blockOut, &bitPos, colors, numSubsets, colorBits, alphaBits);
	WritePBits(blockOut, &bitPos, pBits, numSubsets, pBitMode);
	WriteIndexes(blockOut, &bitPos, indexes,  numSubsets, partition, indexBits);
	WriteIndexes(blockOut, &bitPos, indexes2, numSubsets, partition, index2Bits);
}

void jrcEncodeBc7Block(uint8_t *blockOut, const uint8_t *rgbaIn)
{
	int fullyOpaque, fullyTransparent, singleColor, singleAlpha;
	int i, secondColor;

	fullyOpaque = 1;
	fullyTransparent = 1;
	singleColor = 1;
	secondColor = 0;
	singleAlpha = 1;
	for (i = 0; i < 16; i++)
	{
		if (rgbaIn[i*4+3] != 255)
			fullyOpaque = 0;
		
		if (rgbaIn[i*4+3] != 0)
			fullyTransparent = 0;
		
		if (rgbaIn[i*4  ] != rgbaIn[0] || rgbaIn[i*4+1] != rgbaIn[1] || rgbaIn[i*4+2] != rgbaIn[2])
		{
			if (singleColor)
			{
				singleColor = 0;
				secondColor = i;
			}
			else
			{
				if (rgbaIn[i*4] != rgbaIn[secondColor*4] || rgbaIn[i*4+1] != rgbaIn[secondColor*4+1] || rgbaIn[i*4+2] != rgbaIn[secondColor*4+2])
				secondColor = 0;
			}
		}

		if (rgbaIn[i*4+3] != rgbaIn[3])
			singleAlpha = 0;
	}

	// fully transparent
	if (fullyTransparent)
	{
		// could just return a fully 0 block, but that could glitch when mode 8 is actually used
		blockOut[0] = 0x40; // mode 6;
		for (i = 1; i < 15; i++)
			blockOut[i] = 0;

		return;
	}

	// single color and single alpha, encode as mode 6
	// this is an accurate color unless one or more components are < 2
	if (singleColor && singleAlpha)
	{
		int bitPos = 0;
		int endpoint, component;
		int needInterp;

		SetBits(blockOut, &bitPos, BIT_SHIFT(6), 7); // mode 6

		needInterp = 0;
		for (component = 0; component < 4; component++)
		{
			uint8_t c7 = rgbaIn[component] >> 1;

			if ((rgbaIn[component] & 1) && c7)
			{
				needInterp = 1;
				SetBits(blockOut, &bitPos, c7 - 1, 7);
			}
			else
				SetBits(blockOut, &bitPos, c7, 7);

			SetBits(blockOut, &bitPos, c7, 7);
		}

		// pBits are always 1
		for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
			SetBits(blockOut, &bitPos, 1, 1);

		if (needInterp)
		{
			// all index 7
			SetBits(blockOut, &bitPos, 7, 3);
			for (i = 1; i < 16; i++)
				SetBits(blockOut, &bitPos, 7, 4);
		}
		else
			SetBits(blockOut, &bitPos, 0, 63); // all index 0

		return;
	}
	
	// two color and single alpha, encode as mode 6
	// colors are 7 bit accurate
	if (secondColor && singleAlpha)
	{
		int bitPos = 0;
		int endpoint, component;
		
		SetBits(blockOut, &bitPos, BIT_SHIFT(6), 7); // mode 6
		for (component = 0; component < 4; component++)
		{
			SetBits(blockOut, &bitPos, rgbaIn[component] >> 1, 7);
			SetBits(blockOut, &bitPos, rgbaIn[secondColor*4+component] >> 1, 7);
		}

		// pBits are always 1
		for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
			SetBits(blockOut, &bitPos, 1, 1);

		// first index is 0, because first color is at 0
		SetBits(blockOut, &bitPos, 0, 3);
		for (i = 1; i < 16; i++)
			if (rgbaIn[i*4] == rgbaIn[0] && rgbaIn[i*4+1] == rgbaIn[1] && rgbaIn[i*4+2] == rgbaIn[2])
				SetBits(blockOut, &bitPos, 0, 4);
			else
				SetBits(blockOut, &bitPos, 0xF, 4);

		return;
	}

	// opaque, pick an opaque mode based on color variance
	if (fullyOpaque)
	{
		float minYcbcr[3], maxYcbcr[3], diff[3], variance;
		int i, component;

		minYcbcr[0] = minYcbcr[1] = minYcbcr[2] = 1.0f;
		maxYcbcr[0] = maxYcbcr[1] = maxYcbcr[2] = 0.0f;

		for (i = 0; i < 16; i++)
		{
			float pixelYcbcr[3];

			Rgb8ToYcbcr32f(pixelYcbcr, &rgbaIn[i*4]);
			
			for (component = 0; component < 3; component++)
			{
				if (minYcbcr[component] > pixelYcbcr[component]) minYcbcr[component] = pixelYcbcr[component];
				if (maxYcbcr[component] < pixelYcbcr[component]) maxYcbcr[component] = pixelYcbcr[component];
			}
		}
		
		for (component = 0; component < 3; component++)
			diff[component] = maxYcbcr[component] - minYcbcr[component];
		
		variance = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];

		if (variance < 0.6f)
			EncodeBptcBlockMode(blockOut, rgbaIn, 3);
		else
			EncodeBptcBlockMode(blockOut, rgbaIn, 2);
		return;
	}

	// just encode as a mode 4 block
	EncodeBptcBlockMode(blockOut, rgbaIn, 4);
}
#endif
