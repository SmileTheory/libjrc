#ifndef INCLUDE_JRC_BC7_H
#define INCLUDE_JRC_BC7_H

void jrcDecodeBc7Block(unsigned char *rgbaOut, const unsigned char *blockIn);
void jrcEncodeBc7Block(unsigned char *blockOut, const unsigned char *rgbaIn);

#endif
#ifdef JRC_BC7_IMPLEMENTATION
#include <math.h>

typedef unsigned char  ui8_t;
typedef unsigned short ui16_t;
typedef unsigned int   ui32_t;

#define BIT_MASK(x)  (0xFFFFFFFF >> (32 - (x)))
#define BIT_SHIFT(x) (1 << (x))

#define Vec2Add(r, a, b) ((r)[0] = (a)[0] + (b)[0], (r)[1] = (a)[1] + (b)[1])
#define Vec3Add(r, a, b) ((r)[0] = (a)[0] + (b)[0], (r)[1] = (a)[1] + (b)[1], (r)[2] = (a)[2] + (b)[2])
#define Vec4Add(r, a, b) ((r)[0] = (a)[0] + (b)[0], (r)[1] = (a)[1] + (b)[1], (r)[2] = (a)[2] + (b)[2], (r)[3] = (a)[3] + (b)[3]))

#define Vec2Subtract(r, a, b) ((r)[0] = (a)[0] - (b)[0], (r)[1] = (a)[1] - (b)[1])
#define Vec3Subtract(r, a, b) ((r)[0] = (a)[0] - (b)[0], (r)[1] = (a)[1] - (b)[1], (r)[2] = (a)[2] - (b)[2])
#define Vec4Subtract(r, a, b) ((r)[0] = (a)[0] - (b)[0], (r)[1] = (a)[1] - (b)[1], (r)[2] = (a)[2] - (b)[2], (r)[3] = (a)[3] - (b)[3]))

#define Vec2Clear(r) ((r)[0] = (r)[1] = 0)
#define Vec3Clear(r) ((r)[0] = (r)[1] = (r)[2] = 0)
#define Vec4Clear(r) ((r)[0] = (r)[1] = (r)[2] = (r)[3] = 0)

#define Vec2Identity(r) ((r)[0] = (r)[1] = 1)
#define Vec3Identity(r) ((r)[0] = (r)[1] = (r)[2] = 1)
#define Vec4Identity(r) ((r)[0] = (r)[1] = (r)[2] = (r)[3] = 1)

#define Vec3Copy(r, a) ((r)[0] = (a)[0], (r)[1] = (a)[1], (r)[2] = (a)[2])
#define Vec4Copy(r, a) ((r)[0] = (a)[0], (r)[1] = (a)[1], (r)[2] = (a)[2], (r)[3] = (a)[3])

#define Vec3Equal(a, b) ((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2])
#define Vec4Equal(a, b) ((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2] && (a)[3] == (b)[3])

#define Vec3Scale(r, s, v) ((r)[0] = (s) * (v)[0], (r)[1] = (s) * (v)[1], (r)[2] = (s) * (v)[2])

#define DotProduct3(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#define ABS(x)   ((x) < 0 ? -(x) : (x))

#define NUM_MODES 8
#define NUM_ENDPOINTS 2
#define MAX_SUBSETS 3
#define MAX_INTERPS 16
#define MAX_PARTITIONS 64
#define NUM_PIXELS 16

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

static ui8_t modeInfo[NUM_MODES][MODEINFO_NUM_MODEINFOS] =
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

static ui8_t partitionSubsets[2 * MAX_PARTITIONS * NUM_PIXELS] = 
	{0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
	 0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,
	 0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,
	 0,0,0,1,0,0,1,1,0,0,1,1,0,1,1,1,
	 0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,
	 0,0,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	 0,0,0,1,0,0,1,1,0,1,1,1,1,1,1,1,
	 0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,
	 0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,
	 0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	 0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,
	 0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,
	 0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,
	 0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
	 0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	 0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
	 0,0,0,0,1,0,0,0,1,1,1,0,1,1,1,1,
	 0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,
	 0,1,1,1,0,0,1,1,0,0,0,1,0,0,0,0,
	 0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,
	 0,0,0,0,1,0,0,0,1,1,0,0,1,1,1,0,
	 0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,
	 0,1,1,1,0,0,1,1,0,0,1,1,0,0,0,1,
	 0,0,1,1,0,0,0,1,0,0,0,1,0,0,0,0,
	 0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0,
	 0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,
	 0,0,1,1,0,1,1,0,0,1,1,0,1,1,0,0,
	 0,0,0,1,0,1,1,1,1,1,1,0,1,0,0,0,
	 0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	 0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,
	 0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,
	 0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
	 0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,
	 0,1,0,1,1,0,1,0,0,1,0,1,1,0,1,0,
	 0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0,
	 0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,
	 0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,0,
	 0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,
	 0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,1,
	 0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,
	 0,0,0,1,0,0,1,1,1,1,0,0,1,0,0,0,
	 0,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0,
	 0,0,1,1,1,0,1,1,1,1,0,1,1,1,0,0,
	 0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	 0,0,1,1,1,1,0,0,1,1,0,0,0,0,1,1,
	 0,1,1,0,0,1,1,0,1,0,0,1,1,0,0,1,
	 0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,
	 0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,0,
	 0,0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,
	 0,0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,
	 0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,0,
	 0,1,1,0,1,1,0,0,1,0,0,1,0,0,1,1,
	 0,0,1,1,0,1,1,0,1,1,0,0,1,0,0,1,
	 0,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0,
	 0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,0,
	 0,1,1,0,1,1,0,0,1,1,0,0,1,0,0,1,
	 0,1,1,0,0,0,1,1,0,0,1,1,1,0,0,1,
	 0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,
	 0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,1,
	 0,0,0,0,1,1,1,1,0,0,1,1,0,0,1,1,
	 0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0,
	 0,0,1,0,0,0,1,0,1,1,1,0,1,1,1,0,
	 0,1,0,0,0,1,0,0,0,1,1,1,0,1,1,1,

	 0,0,1,1,0,0,1,1,0,2,2,1,2,2,2,2,
	 0,0,0,1,0,0,1,1,2,2,1,1,2,2,2,1,
	 0,0,0,0,2,0,0,1,2,2,1,1,2,2,1,1,
	 0,2,2,2,0,0,2,2,0,0,1,1,0,1,1,1,
	 0,0,0,0,0,0,0,0,1,1,2,2,1,1,2,2,
	 0,0,1,1,0,0,1,1,0,0,2,2,0,0,2,2,
	 0,0,2,2,0,0,2,2,1,1,1,1,1,1,1,1,
	 0,0,1,1,0,0,1,1,2,2,1,1,2,2,1,1,
	 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,
	 0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,
	 0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,
	 0,0,1,2,0,0,1,2,0,0,1,2,0,0,1,2,
	 0,1,1,2,0,1,1,2,0,1,1,2,0,1,1,2,
	 0,1,2,2,0,1,2,2,0,1,2,2,0,1,2,2,
	 0,0,1,1,0,1,1,2,1,1,2,2,1,2,2,2,
	 0,0,1,1,2,0,0,1,2,2,0,0,2,2,2,0,
	 0,0,0,1,0,0,1,1,0,1,1,2,1,1,2,2,
	 0,1,1,1,0,0,1,1,2,0,0,1,2,2,0,0,
	 0,0,0,0,1,1,2,2,1,1,2,2,1,1,2,2,
	 0,0,2,2,0,0,2,2,0,0,2,2,1,1,1,1,
	 0,1,1,1,0,1,1,1,0,2,2,2,0,2,2,2,
	 0,0,0,1,0,0,0,1,2,2,2,1,2,2,2,1,
	 0,0,0,0,0,0,1,1,0,1,2,2,0,1,2,2,
	 0,0,0,0,1,1,0,0,2,2,1,0,2,2,1,0,
	 0,1,2,2,0,1,2,2,0,0,1,1,0,0,0,0,
	 0,0,1,2,0,0,1,2,1,1,2,2,2,2,2,2,
	 0,1,1,0,1,2,2,1,1,2,2,1,0,1,1,0,
	 0,0,0,0,0,1,1,0,1,2,2,1,1,2,2,1,
	 0,0,2,2,1,1,0,2,1,1,0,2,0,0,2,2,
	 0,1,1,0,0,1,1,0,2,0,0,2,2,2,2,2,
	 0,0,1,1,0,1,2,2,0,1,2,2,0,0,1,1,
	 0,0,0,0,2,0,0,0,2,2,1,1,2,2,2,1,
	 0,0,0,0,0,0,0,2,1,1,2,2,1,2,2,2,
	 0,2,2,2,0,0,2,2,0,0,1,2,0,0,1,1,
	 0,0,1,1,0,0,1,2,0,0,2,2,0,2,2,2,
	 0,1,2,0,0,1,2,0,0,1,2,0,0,1,2,0,
	 0,0,0,0,1,1,1,1,2,2,2,2,0,0,0,0,
	 0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
	 0,1,2,0,2,0,1,2,1,2,0,1,0,1,2,0,
	 0,0,1,1,2,2,0,0,1,1,2,2,0,0,1,1,
	 0,0,1,1,1,1,2,2,2,2,0,0,0,0,1,1,
	 0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,
	 0,0,0,0,0,0,0,0,2,1,2,1,2,1,2,1,
	 0,0,2,2,1,1,2,2,0,0,2,2,1,1,2,2,
	 0,0,2,2,0,0,1,1,0,0,2,2,0,0,1,1,
	 0,2,2,0,1,2,2,1,0,2,2,0,1,2,2,1,
	 0,1,0,1,2,2,2,2,2,2,2,2,0,1,0,1,
	 0,0,0,0,2,1,2,1,2,1,2,1,2,1,2,1,
	 0,1,0,1,0,1,0,1,0,1,0,1,2,2,2,2,
	 0,2,2,2,0,1,1,1,0,2,2,2,0,1,1,1,
	 0,0,0,2,1,1,1,2,0,0,0,2,1,1,1,2,
	 0,0,0,0,2,1,1,2,2,1,1,2,2,1,1,2,
	 0,2,2,2,0,1,1,1,0,1,1,1,0,2,2,2,
	 0,0,0,2,1,1,1,2,1,1,1,2,0,0,0,2,
	 0,1,1,0,0,1,1,0,0,1,1,0,2,2,2,2,
	 0,0,0,0,0,0,0,0,2,1,1,2,2,1,1,2,
	 0,1,1,0,0,1,1,0,2,2,2,2,2,2,2,2,
	 0,0,2,2,0,0,1,1,0,0,1,1,0,0,2,2,
	 0,0,2,2,1,1,2,2,1,1,2,2,0,0,2,2,
	 0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,2,
	 0,0,0,2,0,0,0,1,0,0,0,2,0,0,0,1,
	 0,2,2,2,1,2,2,2,0,2,2,2,1,2,2,2,
	 0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,
	 0,1,1,1,2,0,1,1,2,2,0,1,2,2,2,0};

static ui8_t anchors[3][MAX_PARTITIONS] = 
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

static ui8_t interpFactors[3][MAX_INTERPS] =
	{{0, 21, 43, 64,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	 {0,  9, 18, 27, 37, 46, 55, 64,  0,  0,  0,  0,  0,  0,  0,  0},
	 {0,  4,  9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64}};


static ui32_t GetBits(const ui8_t *in, int *offset, int numBits)
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

static void SetBits(ui8_t *out, int *offset, ui32_t value, int numBits)
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

static void DecodeColors(ui8_t color[MAX_SUBSETS][NUM_ENDPOINTS][4], const ui8_t *blockIn, int *bitPos, int numSubsets, int pBitMode, int colorBits, int alphaBits)
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
				ui8_t b = GetBits(blockIn, bitPos, 1);
				ui8_t cpb = b << (7 - colorBits);
				ui8_t apb = b << (7 - alphaBits);

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
			ui8_t b = GetBits(blockIn, bitPos, 1);
			ui8_t cpb = b << (7 - colorBits);
			ui8_t apb = b << (7 - alphaBits);

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

static void Interp64Rgb(ui8_t *r, ui8_t *x, ui8_t *y, int a)
{
	int b = 64 - a;
	
	r[0] = (x[0] * b + y[0] * a + 32) >> 6;
	r[1] = (x[1] * b + y[1] * a + 32) >> 6;
	r[2] = (x[2] * b + y[2] * a + 32) >> 6;
}

static void Interp64Alpha(ui8_t *r, ui8_t *x, ui8_t *y, int a)
{
	int b = 64 - a;

	r[3] = (x[3] * b + y[3] * a + 32) >> 6;
}

static void InterpColors(ui8_t interpColor[MAX_SUBSETS][MAX_INTERPS][4], ui8_t color[MAX_SUBSETS][NUM_ENDPOINTS][4], int numSubsets, int indexSelection, int indexBits, int index2Bits)
{
	int rgbIndexBits, alphaIndexBits, subset, i;

	rgbIndexBits = indexSelection ? index2Bits : indexBits;
	alphaIndexBits = indexSelection ? indexBits : (index2Bits ? index2Bits : indexBits);

	for (subset = 0; subset < numSubsets; subset++)
	{
		for (i = 0; i < BIT_SHIFT(rgbIndexBits); i++)
			Interp64Rgb(interpColor[subset][i], color[subset][0], color[subset][1], interpFactors[rgbIndexBits - 2][i]);

		for (i = 0; i < BIT_SHIFT(alphaIndexBits); i++)
			Interp64Alpha(interpColor[subset][i], color[subset][0], color[subset][1], interpFactors[alphaIndexBits - 2][i]);
	}
}

static void CopyRGB(ui8_t out[4], const ui8_t in[4])
{
	Vec3Copy(out, in);
}

static void CopyAlpha(ui8_t out[4], const ui8_t in[4])
{
	out[3] = in[3];
}

static void CopyRGBA(ui8_t out[4], const ui8_t in[4])
{
	Vec4Copy(out, in);
}

static void DecodeIndexes(ui8_t *rgbaOut, void (*copyFunc)(ui8_t out[4], const ui8_t in[4]), const ui8_t *blockIn, int *bitPos, ui8_t interpColor[MAX_SUBSETS][MAX_INTERPS][4], int numSubsets, int partition, int indexBits)
{
	int i, subset, anchorIndex, colorIndex;
	ui8_t *subsetTable = (numSubsets > 1) ? &partitionSubsets[((numSubsets - 2) * MAX_PARTITIONS + partition) * NUM_PIXELS] : 0;

	for (i = 0; i < NUM_PIXELS; i++)
	{
		subset = subsetTable ? subsetTable[i] : 0;
		anchorIndex = subset ? anchors[subset - ((numSubsets > 2) ? 0 : 1)][partition] : 0;

		colorIndex = GetBits(blockIn, bitPos, indexBits - ((i == anchorIndex) ? 1 : 0));

		copyFunc(rgbaOut, interpColor[subset][colorIndex]);
		rgbaOut += 4;
	}
}

void jrcDecodeBc7Block(ui8_t *rgbaOut, const ui8_t *blockIn)
{
	ui8_t color[MAX_SUBSETS][NUM_ENDPOINTS][4];
	ui8_t interpColor[MAX_SUBSETS][MAX_INTERPS][4];
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
		for (i = 0; i < NUM_PIXELS; i++)
		{
			ui8_t newAlpha          = rgbaOut[i*4+rotation-1];
			rgbaOut[i*4+rotation-1] = rgbaOut[i*4+3];
			rgbaOut[i*4+3]          = newAlpha;
		}
}

// ITU.BT-709 HDTV studio production in Y'CbCr for non-linear signals
// according to http://www.martinreddy.net/gfx/faqs/colorconv.faq
//
// Altered slightly to fit (0..1) and (-0.5..0.5), and make Y linear
static void Rgb8ToYcbcr32f(float ycbcr32f[3], const ui8_t rgb8[3])
{
	float rgb32f[3];

	Vec3Scale(rgb32f, 1.0f / 255.0f, rgb8);

	ycbcr32f[0] =  0.2125f * rgb32f[0] + 0.7154f * rgb32f[1] + 0.0721f * rgb32f[2];
	ycbcr32f[1] = -0.1145f * rgb32f[0] - 0.3855f * rgb32f[1] + 0.5000f * rgb32f[2];
	ycbcr32f[2] =  0.5000f * rgb32f[0] - 0.4542f * rgb32f[1] - 0.0458f * rgb32f[2];

	ycbcr32f[0] = pow(ycbcr32f[0], 2.2);
}

static void Ycbcr32fToRgb8(ui8_t rgb8[3], const float ycbcr32f[3])
{
	float rgb32f[3];
	float y;

	y = pow(ycbcr32f[0], 1.0/2.2);

	rgb32f[0] = y                         + 1.5750f * ycbcr32f[2];
	rgb32f[1] = y - 0.1870f * ycbcr32f[1] - 0.4678f * ycbcr32f[2];
	rgb32f[2] = y + 1.8558f * ycbcr32f[1];

	// Ycbcr allows for values outside (0..1) once converted to RGB, so clamp.
	rgb8[0] = CLAMP(rgb32f[0] * 255, 0, 255);
	rgb8[1] = CLAMP(rgb32f[1] * 255, 0, 255);
	rgb8[2] = CLAMP(rgb32f[2] * 255, 0, 255);
	
}

static float DistanceBetweenYcbcr(float ycbcr1[3], float ycbcr2[3])
{
	float d[3];

	Vec3Subtract(d, ycbcr1, ycbcr2);
	d[0] *= 4.0f;

	return DotProduct3(d, d);
}

static float DistanceBetweenRgb8(const ui8_t *color1, const ui8_t *color2)
{
	float ycbcr1[3], ycbcr2[3];

	Rgb8ToYcbcr32f(ycbcr1, color1);
	Rgb8ToYcbcr32f(ycbcr2, color2);

	return DistanceBetweenYcbcr(ycbcr1, ycbcr2);
}

static float DistanceBetweenRgba8(const ui8_t *color1, const ui8_t *color2)
{
	float alphaDiff;

	alphaDiff = (color1[3] - color2[3]) / 255.0f;
	alphaDiff *= alphaDiff;

	return DistanceBetweenRgb8(color1, color2) + alphaDiff;
}

static float DistanceBetweenAlpha(const ui8_t *color1, const ui8_t *color2)
{
	return ABS(color1[3] - color2[3]);
}

static float CalcVariance(const ui8_t *rgbaIn)
{
	float minYcbcr[3], maxYcbcr[3], rangeYcbcr[3];
	int i, component;

	Vec3Identity(minYcbcr);
	Vec3Clear(maxYcbcr);

	for (i = 0; i < NUM_PIXELS; i++)
	{
		float pixelYcbcr[3];

		Rgb8ToYcbcr32f(pixelYcbcr, &rgbaIn[i*4]);

		for (component = 0; component < 3; component++)
		{
			if (minYcbcr[component] > pixelYcbcr[component]) minYcbcr[component] = pixelYcbcr[component];
			if (maxYcbcr[component] < pixelYcbcr[component]) maxYcbcr[component] = pixelYcbcr[component];
		}
	}

	Vec3Subtract(rangeYcbcr, maxYcbcr, minYcbcr);
	rangeYcbcr[0] *= 4.0f;
	return DotProduct3(rangeYcbcr, rangeYcbcr);
}

static void FindEndpointsRgb(ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], const ui8_t *rgbaIn, int numSubsets, int partition)
{
	ui8_t *subsetTable = (numSubsets > 1) ? &partitionSubsets[((numSubsets - 2) * MAX_PARTITIONS + partition) * NUM_PIXELS] : 0;
	int subset;

	float blockYcbcrs[NUM_PIXELS][3];
	float minYcbcr[numSubsets][3], maxYcbcr[numSubsets][3], avgYcbcr[numSubsets][3];
	float endpointYcbcr[numSubsets][NUM_ENDPOINTS][3];
	int i, component, majorAxes[numSubsets], count[numSubsets], count2[numSubsets];

	for (i = 0; i < NUM_PIXELS; i++)
		Rgb8ToYcbcr32f(blockYcbcrs[i], &rgbaIn[i*4]);

	for (subset = 0; subset < numSubsets; subset++)
	{
		Vec3Identity(minYcbcr[subset]);
		Vec3Clear(maxYcbcr[subset]);
		Vec3Clear(avgYcbcr[subset]);
		count[subset] = 0;
	}

	// find min, max, avg, range of ycbcr per subset
	for (i = 0; i < NUM_PIXELS; i++)
	{
		subset = subsetTable ? subsetTable[i] : 0;

		for (component = 0; component < 3; component++)
		{
			if (minYcbcr[subset][component] > blockYcbcrs[i][component]) minYcbcr[subset][component] = blockYcbcrs[i][component];
			if (maxYcbcr[subset][component] < blockYcbcrs[i][component]) maxYcbcr[subset][component] = blockYcbcrs[i][component];
			avgYcbcr[subset][component] += blockYcbcrs[i][component];
		}
		count[subset]++;
	}

	// finish averages and find major axes
	for (subset = 0; subset < numSubsets; subset++)
	{
		float rangeYcbcr[3];

		Vec3Scale(avgYcbcr[subset], 1.0f / MAX(count[subset], 1), avgYcbcr[subset]);

		Vec3Subtract(rangeYcbcr, maxYcbcr[subset], minYcbcr[subset]);

		// force major axis to luminance unless there's almost no lumi difference
		majorAxes[subset] = 0;
		if (rangeYcbcr[0] < 0.001f)
		{
			majorAxes[subset] = 1;
			if (rangeYcbcr[2] > rangeYcbcr[1])
				majorAxes[subset] = 2;
		}
	}

	for (subset = 0; subset < numSubsets; subset++)
	{
		Vec3Clear(endpointYcbcr[subset][0]);
		Vec3Clear(endpointYcbcr[subset][1]);
		count[subset] = count2[subset] = 0;
	}

	// find average color on either side of average of major axis
	for (i = 0; i < NUM_PIXELS; i++)
	{
		int majorAxis;
		subset = subsetTable ? subsetTable[i] : 0;

		majorAxis = majorAxes[subset];

		if (blockYcbcrs[i][majorAxis] <= avgYcbcr[subset][majorAxis])
		{
			Vec3Add(endpointYcbcr[subset][0], endpointYcbcr[subset][0], blockYcbcrs[i]);
			count[subset]++;
		}

		if (blockYcbcrs[i][majorAxis] >= avgYcbcr[subset][majorAxis])
		{
			Vec3Add(endpointYcbcr[subset][1], endpointYcbcr[subset][1], blockYcbcrs[i]);
			count2[subset]++;
		}
	}

	for (subset = 0; subset < numSubsets; subset++)
	{
		int majorAxis = majorAxes[subset];

		Vec3Scale(endpointYcbcr[subset][0], 1.0f / MAX(count[subset],  1), endpointYcbcr[subset][0]);
		Vec3Scale(endpointYcbcr[subset][1], 1.0f / MAX(count2[subset], 1), endpointYcbcr[subset][1]);

		// nudge major axis away from center
		endpointYcbcr[subset][0][majorAxis] = minYcbcr[subset][majorAxis] * 0.5f + endpointYcbcr[subset][0][majorAxis] * 0.5f;
		endpointYcbcr[subset][1][majorAxis] = maxYcbcr[subset][majorAxis] * 0.5f + endpointYcbcr[subset][1][majorAxis] * 0.5f;

		// convert ycbcr to endpoint colors
		Ycbcr32fToRgb8(colors[subset][0], endpointYcbcr[subset][0]);
		Ycbcr32fToRgb8(colors[subset][1], endpointYcbcr[subset][1]);
	}
}

static void FindEndpointsAlpha(ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], const ui8_t *rgbaIn, int numSubsets, int partition)
{
	ui8_t *subsetTable = (numSubsets > 1) ? &partitionSubsets[((numSubsets - 2) * MAX_PARTITIONS + partition) * NUM_PIXELS] : 0;
	int subset, i;

	for (subset = 0; subset < numSubsets; subset++)
	{
		colors[subset][0][3] = 255;
		colors[subset][1][3] = 0;
	}

	for (i = 0; i < NUM_PIXELS; i++)
	{
		subset = subsetTable ? subsetTable[i] : 0;

		if (rgbaIn[i*4+3] < colors[subset][0][3]) colors[subset][0][3] = rgbaIn[i*4+3];
		if (rgbaIn[i*4+3] > colors[subset][1][3]) colors[subset][1][3] = rgbaIn[i*4+3];
	}
}

static void EncodeIndexes(int indexes[NUM_PIXELS], int indexBits, const ui8_t *rgbaIn, ui8_t interpColors[MAX_SUBSETS][MAX_INTERPS][4], float (*compFunc)(const ui8_t *a, const ui8_t *b), int numSubsets, int partition)
{
	ui8_t *subsetTable = (numSubsets > 1) ? &partitionSubsets[((numSubsets - 2) * MAX_PARTITIONS + partition) * NUM_PIXELS] : 0;
	int i, j;

	if (!indexBits)
		return;

	for (i = 0; i < NUM_PIXELS; i++)
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

static void CorrectEndpoints(ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], ui8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int numSubsets, int colorBits, int alphaBits, int pBitsMode)
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

static void SwapRgb(ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], ui8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int subset)
{
	ui8_t swap;

	swap = colors[subset][0][0]; colors[subset][0][0] = colors[subset][1][0]; colors[subset][1][0] = swap;
	swap = colors[subset][0][1]; colors[subset][0][1] = colors[subset][1][1]; colors[subset][1][1] = swap;
	swap = colors[subset][0][2]; colors[subset][0][2] = colors[subset][1][2]; colors[subset][1][2] = swap;
	swap = pBits[subset][0];     pBits[subset][0] = pBits[subset][1];         pBits[subset][1] = swap;
}

static void SwapRgba(ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], ui8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int subset)
{
	ui8_t swap;

	swap = colors[subset][0][0]; colors[subset][0][0] = colors[subset][1][0]; colors[subset][1][0] = swap;
	swap = colors[subset][0][1]; colors[subset][0][1] = colors[subset][1][1]; colors[subset][1][1] = swap;
	swap = colors[subset][0][2]; colors[subset][0][2] = colors[subset][1][2]; colors[subset][1][2] = swap;
	swap = colors[subset][0][3]; colors[subset][0][3] = colors[subset][1][3]; colors[subset][1][3] = swap;
	swap = pBits[subset][0];     pBits[subset][0] = pBits[subset][1];         pBits[subset][1] = swap;
}

static void SwapAlpha(ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], ui8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int subset)
{
	ui8_t swap;

	swap = colors[subset][0][3]; colors[subset][0][3] = colors[subset][1][3]; colors[subset][1][3] = swap;
}

static void FixAnchorIndexes(ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], ui8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int indexes[NUM_PIXELS], int numSubsets, int partition, int indexBits, void (*swapFunc)(ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], ui8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int subset))
{
	ui8_t *subsetTable = (numSubsets > 1) ? &partitionSubsets[((numSubsets - 2) * MAX_PARTITIONS + partition) * NUM_PIXELS] : 0;
	int subset, i;

	if (!indexBits)
		return;

	for (subset = 0; subset < numSubsets; subset++)
	{
		int anchorIndex = subset ? anchors[subset - ((numSubsets > 2) ? 0 : 1)][partition] : 0;

		if (indexes[anchorIndex] & BIT_SHIFT(indexBits - 1))
		{
			for (i = 0; i < NUM_PIXELS; i++)
			{
				if ((subsetTable ? subsetTable[i] : 0) != subset)
					continue;

				indexes[i] = BIT_MASK(indexBits) - indexes[i];
			}

			swapFunc(colors, pBits, subset);
		}
	}
}

static void WriteRgba(ui8_t *blockOut, int *bitPos, ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4], int numSubsets, int colorBits, int alphaBits)
{
	int component, subset, endpoint;
	int cBits[4] = {colorBits, colorBits, colorBits, alphaBits};

	for (component = 0; component < 4; component++)
		for (subset = 0; subset < numSubsets; subset++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				SetBits(blockOut, bitPos, colors[subset][endpoint][component] >> (8 - cBits[component]), cBits[component]);
}

static void WritePBits(ui8_t *blockOut, int *bitPos, ui8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS], int numSubsets, int pBitMode)
{
	int subset, endpoint;

	if (!pBitMode)
		return;

	for (subset = 0; subset < numSubsets; subset++)
		for (endpoint = 0; endpoint < 3 - pBitMode; endpoint++)
				SetBits(blockOut, bitPos, pBits[subset][endpoint], 1);
}

static void WriteIndexes(ui8_t *blockOut, int *bitPos, int indexes[NUM_PIXELS], int numSubsets, int partition, int indexBits)
{
	ui8_t *subsetTable = (numSubsets > 1) ? &partitionSubsets[((numSubsets - 2) * MAX_PARTITIONS + partition) * NUM_PIXELS] : 0;
	int i;

	if (!indexBits)
		return;

	for (i = 0; i < NUM_PIXELS; i++)
	{
		int subset = subsetTable ? subsetTable[i] : 0;
		int anchorIndex = subset ? anchors[subset - ((numSubsets > 2) ? 0 : 1)][partition] : 0;

		SetBits(blockOut, bitPos, indexes[i], indexBits - ((i == anchorIndex) ? 1 : 0));
	}
}

// pick a partition based on the largest distance between average ycbcr of subsets
int PickPartition(const ui8_t *rgbaIn, int numSubsets, int partitionBits, float *distanceOut)
{
	int partition, numPartitions, bestPartition, i;
	float bestDistance, blockYcbcrs[NUM_PIXELS][3];
	ui8_t *subsetTable;

	if (numSubsets < 2)
		return 0;

	numPartitions = BIT_SHIFT(partitionBits);

	for (i = 0; i < NUM_PIXELS; i++)
		Rgb8ToYcbcr32f(blockYcbcrs[i], &rgbaIn[i*4]);

	bestPartition = 0;
	bestDistance = 0.0f;

	subsetTable = &partitionSubsets[(numSubsets - 2) * MAX_PARTITIONS * NUM_PIXELS];

	for (partition = 0; partition < numPartitions; partition++)
	{
		float distance, avgYcbcrs[MAX_SUBSETS][3];
		int counts[MAX_SUBSETS];
		int subset;

		for (subset = 0; subset < numSubsets; subset++)
		{
			counts[subset] = 0;
			Vec3Clear(avgYcbcrs[subset]);
		}

		for (i = 0; i < NUM_PIXELS; i++)
		{
			subset = *subsetTable++;
			Vec3Add(avgYcbcrs[subset], avgYcbcrs[subset], blockYcbcrs[i]);
			counts[subset]++;
		}

		for (subset = 0; subset < numSubsets; subset++)
			Vec3Scale(avgYcbcrs[subset], 1.0f / counts[subset], avgYcbcrs[subset]);

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

static void EncodeBptcBlockMode(ui8_t *blockOut, const ui8_t *rgbaIn, int mode)
{
	ui8_t colors[MAX_SUBSETS][NUM_ENDPOINTS][4];
	ui8_t interpColors[MAX_SUBSETS][MAX_INTERPS][4];
	ui8_t pBits[MAX_SUBSETS][NUM_ENDPOINTS];
	int indexes[NUM_PIXELS], indexes2[NUM_PIXELS];

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

	partition = PickPartition(rgbaIn, numSubsets, partitionBits, 0);
	rotation = 0; // FIXME: use rotationBits
	indexSelection = (mode == 4) ? 1 : 0; // FIXME: use indexSelectionBits

	FindEndpointsRgb(colors, rgbaIn, numSubsets, partition);
	FindEndpointsAlpha(colors, rgbaIn, numSubsets, partition);
	CorrectEndpoints(colors, pBits, numSubsets, colorBits, alphaBits, pBitMode);
	InterpColors(interpColors, colors, numSubsets, indexSelection, indexBits, index2Bits);

	EncodeIndexes(indexes,  indexBits,  rgbaIn, interpColors, mode > 5 ? DistanceBetweenRgba8 : (indexSelection ? DistanceBetweenAlpha : DistanceBetweenRgb8), numSubsets, partition);
	EncodeIndexes(indexes2, index2Bits, rgbaIn, interpColors, indexSelection ? DistanceBetweenRgb8 : DistanceBetweenAlpha, numSubsets, partition);
	FixAnchorIndexes(colors, pBits, indexes,  numSubsets, partition, indexBits,  mode > 5 ? SwapRgba : (indexSelection ? SwapAlpha : SwapRgb));
	FixAnchorIndexes(colors, pBits, indexes2, numSubsets, partition, index2Bits, indexSelection ? SwapRgb : SwapAlpha);

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

void jrcEncodeBc7Block(ui8_t *blockOut, const ui8_t *rgbaIn)
{
	int fullyOpaque, fullyTransparent, singleColor;
	int i, secondColor;

	fullyOpaque = fullyTransparent = singleColor = 1;
	secondColor = 0;
	for (i = 0; i < NUM_PIXELS; i++)
	{
		if (rgbaIn[i*4+3] != 255)
			fullyOpaque = 0;
		
		if (rgbaIn[i*4+3] != 0)
			fullyTransparent = 0;
		
		if (!Vec4Equal(&rgbaIn[0], &rgbaIn[i*4]))
		{
			if (singleColor)
				secondColor = i;
			else if (!Vec4Equal(&rgbaIn[secondColor*4], &rgbaIn[i*4]))
				secondColor = 0;

			singleColor = 0;
		}
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

	// single color, encode as mode 6
	// this is 8 bit color and 7 bit alpha accurate unless one or more components are < 2
	if (singleColor)
	{
		int bitPos = 0;
		int endpoint, component;
		int needInterp;

		SetBits(blockOut, &bitPos, BIT_SHIFT(6), 7); // mode 6

		needInterp = 0;
		for (component = 0; component < 4; component++)
		{
			ui8_t c0, c1;
			c0 = c1 = rgbaIn[component] >> 1;

			if ((rgbaIn[component] & 1) && c0)
			{
				needInterp = 1;
				c0--;
			}

			SetBits(blockOut, &bitPos, c0, 7);
			SetBits(blockOut, &bitPos, c1, 7);
		}

		// pBits are always 1
		for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
			SetBits(blockOut, &bitPos, 1, 1);

		if (needInterp)
		{
			// all index 7
			SetBits(blockOut, &bitPos, 7, 3);
			for (i = 1; i < NUM_PIXELS; i++)
				SetBits(blockOut, &bitPos, 7, 4);
		}
		else
			SetBits(blockOut, &bitPos, 0, 63); // all index 0

		return;
	}

	// two color, encode as mode 6
	// colors are 7 bit accurate
	if (secondColor)
	{
		int bitPos = 0;
		int endpoint, component;

		SetBits(blockOut, &bitPos, BIT_SHIFT(6), 7); // mode 6
		for (component = 0; component < 4; component++)
			for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
				SetBits(blockOut, &bitPos, rgbaIn[endpoint * secondColor * 4 + component] >> 1, 7);

		for (endpoint = 0; endpoint < NUM_ENDPOINTS; endpoint++)
		{
			const ui8_t *color = &rgbaIn[endpoint * secondColor * 4];
			ui8_t pBit;

			if (color[3] == 0)
				pBit = 0;
			else if (color[3] == 255)
				pBit = 1;
			else
				pBit = (((color[0] & 1) + (color[1] & 1) + (color[2] & 1) + (color[3] & 1)) >> 1) ? 1 : 0;
			
			SetBits(blockOut, &bitPos, pBit, 1);
		}

		// first index is 0, because first color is at 0
		SetBits(blockOut, &bitPos, 0, 3);
		for (i = 1; i < NUM_PIXELS; i++)
			if (Vec3Equal(&rgbaIn[0], &rgbaIn[i*4]))
				SetBits(blockOut, &bitPos, 0, 4);
			else
				SetBits(blockOut, &bitPos, 0xF, 4);

		return;
	}

	// opaque, pick an opaque mode based on color variance
	if (fullyOpaque)
	{
		float variance = CalcVariance(rgbaIn);

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
