#ifndef BITOPS_H
#define BITOPS_H

#include "define.h"

////////////////////////////////////////////
#if defined(IS_WINDOWS) && defined(IS_64BIT)
__forceinline uint32 LSB (uint64 b)
    {
    uint32 index;
    _BitScanForward64(& index, b);
    return (index);
    }
#ifdef HW_POPCNT
__forceinline uint32 popcount (uint64 b)
    {
    return _mm_popcnt_u64(b);
    }
#else
__forceinline uint32 popcount (uint64 b)
    {
    b = b - ((b >> 1) & 0x5555555555555555);
    b = (b & 0x3333333333333333) + ((b >> 2) & 0x3333333333333333);
    b = (b + (b >> 4)) & 0x0f0f0f0f0f0f0f0f;
    return (b * 0x0101010101010101) >> 56;
    }
#endif
#endif
////////////////////////////////////////////
#if defined(IS_WINDOWS) && defined(IS_32BIT)
__forceinline uint32 LSB (uint64 b)
    {
	uint32 lo = (uint32)b;
	uint32 hi = (uint32)(b >> 32);
	DWORD id;
	if (lo)
		_BitScanForward(&id, lo);
	else
		{
		_BitScanForward(&id, hi);
		id += 32;
		}
	return (uint32)id;
    }
#ifdef HW_POPCNT
__forceinline uint32 popcount (uint64 b)
    {
	uint32 lo = (uint32)b;
	uint32 hi = (uint32)(b >> 32);
	return _mm_popcnt_u32(lo) + _mm_popcnt_u32(hi);
    }
#else
__forceinline uint32 popcount (uint64 b)
    {
    unsigned int b1, b2;
    b1 = (unsigned int)(b & 0xFFFFFFFF);
    b1 -= (b1 >> 1) & 0x55555555;
    b1 = (b1 & 0x33333333) + ((b1 >> 2) & 0x33333333);
    b1 = (b1 + (b1 >> 4)) & 0x0F0F0F0F;
    b2 = (unsigned int)(b >> 32);
    b2 -= (b2 >> 1) & 0x55555555;
    b2 = (b2 & 0x33333333) + ((b2 >> 2) & 0x33333333);
    b2 = (b2 + (b2 >> 4)) & 0x0F0F0F0F;
    return ((b1 * 0b01010101) >> 24) + ((b2 * 0b01010101) >> 24);
    }
#endif
#endif
////////////////////////////////////////////
#if !defined(IS_WINDOWS) && defined(IS_64BIT)
__forceinline uint32 LSB (uint64 b)
    {
    uint64 x;
    asm ("bsfq %1,%0\n": "=&r" (x) : "r" (b));
    return x;
    }

__forceinline uint32 popcount (uint64 b)
    {
    const uint64 mask1 = (0x5555555555555555ULL);
    const uint64 mask2 = (0x3333333333333333ULL);
    const uint64 mask4 = (0x0f0f0f0f0f0f0f0fULL);
    const uint64 mask8 = (0x0101010101010101ULL);

    register bb_t x = b;
    x = x - ((x >> 1) & mask1);
    x = (x & mask2)+((x >> 2) & mask2);
    x = (x + (x >> 4)) & mask4;
    return uint32((x * mask8) >> 56);
    }
#endif
////////////////////////////////////////////
#if !defined(IS_WINDOWS) && defined(IS_32BIT)
__forceinline uint32 LSB (uint64 b)
    {
    static const uint32 lsb_64_table[64] =
        {
		0,  47,  1, 56, 48, 27,  2, 60,
		57, 49, 41, 37, 28, 16,  3, 61,
		54, 58, 35, 52, 50, 42, 21, 44,
		38, 32, 29, 23, 17, 11,  4, 62,
		46, 55, 26, 59, 40, 36, 15, 53,
		34, 51, 20, 43, 31, 22, 10, 45,
		25, 39, 14, 33, 19, 30,  9, 24,
		13, 18,  8, 12,  7,  6,  5, 63
        };
    return lsb_64_table[((b ^ (b - 1))* LL(0x03f79d71b4cb0a89)) >> 58];
    }
__forceinline uint32 popcount (uint64 b)
    {
    const uint64 mask1 = (0x5555555555555555ULL);
    const uint64 mask2 = (0x3333333333333333ULL);
    const uint64 mask4 = (0x0f0f0f0f0f0f0f0fULL);
    const uint64 mask8 = (0x00ff00ff00ff00ffULL);

    register bb_t x = b;
    x = (x & mask1)+((x >> 1) & mask1);
    x = (x & mask2)+((x >> 2) & mask2);
    x = (x & mask4)+((x >> 4) & mask4);
    x = (x & mask8)+((x >> 8) & mask8);

    uint32 y = uint32(x) + uint32(x >> 32);
    return uint8(y + (y >> 16));
    }
#endif
#endif